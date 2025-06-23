// Copyright Epic Games, Inc. All Rights Reserved.

#include "Car.h"
#include "Components/SkeletalMeshComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"
#include "ChaosWheeledVehicleMovementComponent.h"
#include "PassengerComponent.h"
#include "Components/SphereComponent.h"
#include "Prepper/GamePlay/PrepperGameMode.h"
#include "Prepper/GamePlay/CharacterController/CharacterController.h"
//#include "Prepper/___Legacy/PlayerController/BasePlayerController.h"

#define LOCTEXT_NAMESPACE "VehiclePawn"

DEFINE_LOG_CATEGORY(LogTemplateVehicle);

ACar::ACar()
{
	// construct the front camera boom
	FrontSpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("Front Spring Arm"));
	FrontSpringArm->SetupAttachment(GetMesh());
	FrontSpringArm->TargetArmLength = 0.0f;
	FrontSpringArm->bDoCollisionTest = false;
	FrontSpringArm->bEnableCameraRotationLag = true;
	FrontSpringArm->CameraRotationLagSpeed = 15.0f;
	FrontSpringArm->SetRelativeLocation(FVector(30.0f, 0.0f, 120.0f));

	FrontCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("Front Camera"));
	FrontCamera->SetupAttachment(FrontSpringArm);
	FrontCamera->bAutoActivate = false;

	// construct the back camera boom
	BackSpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("Back Spring Arm"));
	BackSpringArm->SetupAttachment(GetMesh());
	BackSpringArm->TargetArmLength = 650.0f;
	BackSpringArm->SocketOffset.Z = 150.0f;
	BackSpringArm->bDoCollisionTest = false;
	BackSpringArm->bInheritPitch = false;
	BackSpringArm->bInheritRoll = false;
	BackSpringArm->bEnableCameraRotationLag = true;
	BackSpringArm->CameraRotationLagSpeed = 2.0f;
	BackSpringArm->CameraLagMaxDistance = 50.0f;

	BackCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("Back Camera"));
	BackCamera->SetupAttachment(BackSpringArm);

	// Configure the car mesh
	GetMesh()->SetSimulatePhysics(true);
	GetMesh()->SetCollisionProfileName(FName("Vehicle"));
	
	AreaSphere = CreateDefaultSubobject<USphereComponent>("AreaSphere");
	AreaSphere->SetupAttachment(RootComponent);
	AreaSphere->SetCollisionResponseToAllChannels(ECR_Ignore);
	AreaSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	// get the Chaos Wheeled movement component
	ChaosVehicleMovement = CastChecked<UChaosWheeledVehicleMovementComponent>(GetVehicleMovement());
	AActor::SetReplicateMovement(true);
	
	Passenger = CreateDefaultSubobject<UPassengerComponent>("Passenger");

}

void ACar::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		//EnhancedInputComponent->BindAction(SteeringAction, ETriggerEvent::Completed, this, &ACarPawn::Move);
		EnhancedInputComponent->BindAction(BrakeAction, ETriggerEvent::Started, this, &ACar::StartBrake);
		EnhancedInputComponent->BindAction(BrakeAction, ETriggerEvent::Completed, this, &ACar::StopBrake);

		// handbrake 
		EnhancedInputComponent->BindAction(HandbrakeAction, ETriggerEvent::Started, this, &ACar::StartHandbrake);
		EnhancedInputComponent->BindAction(HandbrakeAction, ETriggerEvent::Completed, this, &ACar::StopHandbrake);

		// reset the vehicle 
		EnhancedInputComponent->BindAction(ResetVehicleAction, ETriggerEvent::Triggered, this, &ACar::ResetVehicle);
	}
	else
	{
		UE_LOG(LogTemplateVehicle, Error, TEXT("'%s' Failed to find an Enhanced Input component! This template is built to use the Enhanced Input system. If you intend to use the legacy system, then you will need to update this C++ file."), *GetNameSafe(this));
	}
}

void ACar::BeginPlay()
{
	Super::BeginPlay();
	AreaSphere->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	AreaSphere->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	CurrentHealth = MaxHealth;
	// 게임 시작시 플레이어 UI 동기화(초기화)
	
}

void ACar::Tick(float Delta)
{
	Super::Tick(Delta);

	// add some angular damping if the vehicle is in midair
	bool bMovingOnGround = ChaosVehicleMovement->IsMovingOnGround();
	GetMesh()->SetAngularDamping(bMovingOnGround ? 0.0f : 3.0f);

	// realign the camera yaw to face front
	float CameraYaw = BackSpringArm->GetRelativeRotation().Yaw;
	CameraYaw = FMath::FInterpTo(CameraYaw, 0.0f, Delta, 1.0f);

	BackSpringArm->SetRelativeRotation(FRotator(0.0f, CameraYaw, 0.0f));
}

void ACar::Move(const FInputActionValue& Value, TObjectPtr<APawn> Attempter)
{
	if (!Passenger->IsDriver(Attempter))
	{
		UE_LOG(LogTemp, Warning, TEXT("NOT DRIVER"));
		return;
	}
	
	FVector2D MovementVector = Value.Get<FVector2D>();

	ChaosVehicleMovement->SetSteeringInput(MovementVector.X);
	
	ChaosVehicleMovement->SetThrottleInput(MovementVector.Y);
	ChaosVehicleMovement->SetBrakeInput(-MovementVector.Y);
}

void ACar::Look(const FInputActionValue& Value)
{
	// get the flat angle value for the input 
	float LookValue = Value.Get<float>();

	// add the input
	BackSpringArm->AddLocalRotation(FRotator(0.0f, LookValue, 0.0f));
	
}

void ACar::ChangeCam()
{
	// toggle the active camera flag
	bFrontCameraActive = !bFrontCameraActive;

	FrontCamera->SetActive(bFrontCameraActive);
	BackCamera->SetActive(!bFrontCameraActive);
}

void ACar::Interaction(APlayerCharacter* Target)
{
	
}

void ACar::Interaction(ICharacterController* Target)
{
	if (Passenger->IsFull()) return;
	
	Passenger->AddPassenger(Target->GetPawn());
	Target->Boarding(this);
}

void ACar::InteractionAct(ICharacterController* Target)
{
	Passenger->RemovePassenger(Target->GetPawn());
	Target->GetOff();
}

void ACar::ShowPickUpWidget(bool bShowWidget)
{
	
}

void ACar::StartBrake(const FInputActionValue& Value)
{
	// call the Blueprint hook for the break lights
	BrakeLights(true);
}

void ACar::StopBrake(const FInputActionValue& Value)
{
	// call the Blueprint hook for the break lights
	BrakeLights(false);

	// reset brake input to zero
	ChaosVehicleMovement->SetBrakeInput(0.0f);
}

void ACar::StartHandbrake(const FInputActionValue& Value)
{
	// add the input
	ChaosVehicleMovement->SetHandbrakeInput(true);

	// call the Blueprint hook for the break lights
	BrakeLights(true);
}

void ACar::StopHandbrake(const FInputActionValue& Value)
{
	// add the input
	ChaosVehicleMovement->SetHandbrakeInput(false);

	// call the Blueprint hook for the break lights
	BrakeLights(false);
}

void ACar::ResetVehicle(const FInputActionValue& Value)
{
	// reset to a location slightly above our current one
	FVector ResetLocation = GetActorLocation() + FVector(0.0f, 0.0f, 50.0f);

	// reset to our yaw. Ignore pitch and roll
	FRotator ResetRotation = GetActorRotation();
	ResetRotation.Pitch = 0.0f;
	ResetRotation.Roll = 0.0f;
	
	// teleport the actor to the reset spot and reset physics
	SetActorTransform(FTransform(ResetRotation, ResetLocation, FVector::OneVector), false, nullptr, ETeleportType::TeleportPhysics);

	GetMesh()->SetPhysicsAngularVelocityInDegrees(FVector::ZeroVector);
	GetMesh()->SetPhysicsLinearVelocity(FVector::ZeroVector);
	GetMovementComponent()->ResetMoveState();

	UE_LOG(LogTemp, Error, TEXT("Reset Vehicle"));
}

void ACar::ReceiveDamage(float Damage, AController* InstigatorController, AActor* DamageCauser)
{
	CurrentHealth = FMath::Clamp(CurrentHealth - Damage, 0.f, MaxHealth);

	if(CurrentHealth != 0.f) return;
	APrepperGameMode* PrepperGameMode =  GetWorld()->GetAuthGameMode<APrepperGameMode>();
	
	if(PrepperGameMode == nullptr) return;
	
	//ABasePlayerController* PrepperPlayerController = Cast<ABasePlayerController>(GetController());
	//ABasePlayerController* AttackerController = Cast<ABasePlayerController>(InstigatorController);
	
	//PrepperGameMode->PlayerEliminated(Driver, PrepperPlayerController, AttackerController);
}

#undef LOCTEXT_NAMESPACE