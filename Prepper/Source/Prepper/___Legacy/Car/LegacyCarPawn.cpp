// Copyright Epic Games, Inc. All Rights Reserved.

#include "LegacyCarPawn.h"
#include "Components/SkeletalMeshComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"
#include "ChaosWheeledVehicleMovementComponent.h"
#include "Components/SphereComponent.h"
#include "Net/UnrealNetwork.h"
#include "Prepper/GamePlay/PrepperGameMode.h"
#include "Prepper/GamePlay/CharacterController/CharacterController.h"
#include "Prepper/___Legacy/Character/PlayerCharacter.h"
#include "Prepper/___Legacy/ControlMapper/CarControlMapper.h"
#include "Prepper/___Legacy/PlayerController/BasePlayerController.h"

#define LOCTEXT_NAMESPACE "VehiclePawn"

class APrepperGameMode;

ALegacyCarPawn::ALegacyCarPawn()
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

	Driver = nullptr;

}

void ALegacyCarPawn::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		//EnhancedInputComponent->BindAction(SteeringAction, ETriggerEvent::Completed, this, &ACarPawn::Move);
		EnhancedInputComponent->BindAction(BrakeAction, ETriggerEvent::Started, this, &ALegacyCarPawn::StartBrake);
		EnhancedInputComponent->BindAction(BrakeAction, ETriggerEvent::Completed, this, &ALegacyCarPawn::StopBrake);

		// handbrake 
		EnhancedInputComponent->BindAction(HandbrakeAction, ETriggerEvent::Started, this, &ALegacyCarPawn::StartHandbrake);
		EnhancedInputComponent->BindAction(HandbrakeAction, ETriggerEvent::Completed, this, &ALegacyCarPawn::StopHandbrake);

		// reset the vehicle 
		EnhancedInputComponent->BindAction(ResetVehicleAction, ETriggerEvent::Triggered, this, &ALegacyCarPawn::ResetVehicle);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("'%s' Failed to find an Enhanced Input component! This template is built to use the Enhanced Input system. If you intend to use the legacy system, then you will need to update this C++ file."), *GetNameSafe(this));
	}
}
void ALegacyCarPawn::BeginPlay()
{
	Super::BeginPlay();
	AreaSphere->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	AreaSphere->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	CurrentHealth = MaxHealth;
	// 게임 시작시 플레이어 UI 동기화(초기화)
	
}

void ALegacyCarPawn::Tick(float Delta)
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

void ALegacyCarPawn::Move(const FInputActionValue& Value)
{
	FVector2D MovementVector = Value.Get<FVector2D>();

	ChaosVehicleMovement->SetSteeringInput(MovementVector.X);
	
	ChaosVehicleMovement->SetThrottleInput(MovementVector.Y);
	
	ChaosVehicleMovement->SetBrakeInput(-MovementVector.Y);
}
void ALegacyCarPawn::Look(const FInputActionValue& Value)
{
	// get the flat angle value for the input 
	float LookValue = Value.Get<float>();

	// add the input
	BackSpringArm->AddLocalRotation(FRotator(0.0f, LookValue, 0.0f));

	
}

void ALegacyCarPawn::InteractionAct()
{
	Controller->Possess(Driver);
	
	Driver->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
	Driver->SetActorLocation(GetActorLocation() + FVector(0, 0, 200));
	Driver->SetActorRotation(FRotator().ZeroRotator);
	Driver->SetMovementState(EMovementState::EMS_Idle);

	Driver = nullptr;
}

void ALegacyCarPawn::ChangeCam()
{
	// toggle the active camera flag
	bFrontCameraActive = !bFrontCameraActive;

	FrontCamera->SetActive(bFrontCameraActive);
	BackCamera->SetActive(!bFrontCameraActive);
}

UCameraComponent* ALegacyCarPawn::GetFollowCamera()
{
	return bFrontCameraActive ? FrontCamera : BackCamera;
}

IControlMapper* ALegacyCarPawn::GetControlMapper()
{
	if (!CarControlMapper)
	{
		CarControlMapper = NewObject<UCarControlMapper>(this, MapperClass);
		CarControlMapper->TargetCar = this;
	}
	
	return CarControlMapper;
}

void ALegacyCarPawn::Interaction(APlayerCharacter* Target)
{
	if (Driver) return;
	
	Driver = Target;

	Target->Controller->Possess(this);
	Target->SetMovementState(EMovementState::EMS_Seat);
	
	MulticastInteraction(Target);
}

void ALegacyCarPawn::MulticastInteraction_Implementation(APlayerCharacter* Target)
{
	Driver = Target;
	Target->AttachToActor(this, FAttachmentTransformRules::KeepWorldTransform);
}

void ALegacyCarPawn::Interaction(ICharacterController* Target)
{
	
}

void ALegacyCarPawn::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ALegacyCarPawn, CurrentHealth);
}

void ALegacyCarPawn::ShowPickUpWidget(bool bShowWidget)
{
	
}

void ALegacyCarPawn::StartBrake(const FInputActionValue& Value)
{
	// call the Blueprint hook for the break lights
	BrakeLights(true);
}

void ALegacyCarPawn::StopBrake(const FInputActionValue& Value)
{
	// call the Blueprint hook for the break lights
	BrakeLights(false);

	// reset brake input to zero
	ChaosVehicleMovement->SetBrakeInput(0.0f);
}

void ALegacyCarPawn::StartHandbrake(const FInputActionValue& Value)
{
	// add the input
	ChaosVehicleMovement->SetHandbrakeInput(true);

	// call the Blueprint hook for the break lights
	BrakeLights(true);
}

void ALegacyCarPawn::StopHandbrake(const FInputActionValue& Value)
{
	// add the input
	ChaosVehicleMovement->SetHandbrakeInput(false);

	// call the Blueprint hook for the break lights
	BrakeLights(false);
}

void ALegacyCarPawn::ResetVehicle(const FInputActionValue& Value)
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
	UE_LOG(LogTemp, Error, TEXT("Reset Vehicle"));
}

void ALegacyCarPawn::ReceiveDamage(float Damage, AController* InstigatorController, AActor* DamageCauser)
{
	CurrentHealth = FMath::Clamp(CurrentHealth - Damage, 0.f, MaxHealth);

	if(CurrentHealth != 0.f) return;
	APrepperGameMode* PrepperGameMode =  GetWorld()->GetAuthGameMode<APrepperGameMode>();
	
	if(PrepperGameMode == nullptr) return;
	
	ABasePlayerController* PrepperPlayerController = Cast<ABasePlayerController>(GetController());
	ABasePlayerController* AttackerController = Cast<ABasePlayerController>(InstigatorController);
	PrepperGameMode->PlayerEliminated(Driver, PrepperPlayerController, AttackerController);
}

#undef LOCTEXT_NAMESPACE
