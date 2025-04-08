// Fill out your copyright notice in the Description page of Project Settings.


#include "BCharacter.h"

#include "CharacterAnimInstance.h"
#include "Engine/SkeletalMeshSocket.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Perception/PawnSensingComponent.h"
#include "Prepper/__Legacy/Component/CustomCameraComponent.h"
#include "Prepper/__Legacy/Component/FlexibleSpringArmComponent/FlexibleSpringArmComponent.h"

// Sets default values
ABCharacter::ABCharacter()
{
	PrimaryActorTick.bCanEverTick = true;
	
	IsAiming = false;
	IsSprint = false;
	
	FlexibleCameraBoom = CreateDefaultSubobject<UFlexibleSpringArmComponent>(TEXT("FlexibleCameraBoom"));
	FlexibleCameraBoom->ChangeArmOffsetToTemplate(FString("Default"));
	FlexibleCameraBoom->SetupAttachment(GetMesh());
	FlexibleCameraBoom->bUsePawnControlRotation = true;

	FollowCamera = CreateDefaultSubobject<UCustomCameraComponent>(TEXT("FollowCam"));
	FollowCamera->SetupAttachment(FlexibleCameraBoom, USpringArmComponent::SocketName);
	FollowCamera->bUsePawnControlRotation = false;

	PawnSensing = CreateDefaultSubobject<UPawnSensingComponent>(TEXT("PawnSensing"));
	PawnSensing->SetComponentTickEnabled(false);
	
	bUseControllerRotationYaw = false;
}

void ABCharacter::PlayAnim(UAnimMontage* Montage, const FName& SectionName) const
{
	if (Montage == nullptr) return;
	
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (!AnimInstance) return;
	
	AnimInstance->Montage_Play(Montage);
	if (SectionName.Compare("") == 0) return;
	
	AnimInstance->Montage_JumpToSection(SectionName);
}

float ABCharacter::GetSpeed() const
{
	if (IsAiming) return AimMovementSpeed;
	if (IsSprint) return SprintSpeed;
	return WalkSpeed;
}

void ABCharacter::GetLookDirection(FVector& Start, FVector& Forward) const
{
	Start = FollowCamera->GetComponentLocation();
	Forward = FollowCamera->GetForwardVector();
}

void ABCharacter::AttachActorAtSocket(FName SocketName, AActor* TargetActor) const
{
	if(const USkeletalMeshSocket* TargetSocket
		= GetMesh()->GetSocketByName(SocketName))
	{
		//AttachedActor.Add(TargetActor);
		TargetSocket->AttachActor(TargetActor, GetMesh());
	}
	UE_LOG(LogTemp, Warning, TEXT("Attach %s"), *SocketName.ToString());
}

void ABCharacter::SetEquippedWeaponType(const EWeaponType WeaponType)
{
	bUseControllerRotationYaw = WeaponType != EWeaponType::EWT_MAX;
	Cast<UCharacterAnimInstance>(GetMesh()->GetAnimInstance())->SetEquippedWeaponType(WeaponType);
}

TObjectPtr<UPawnSensingComponent> ABCharacter::GetPawnSensing() const
{
	return PawnSensing;
}

void ABCharacter::BeginPlay()
{
	Super::BeginPlay();
	
}

void ABCharacter::Move(float X, float Z)
{
	const FRotator Rotation = Controller->GetControlRotation();
	const FRotator YawRotation(0, Rotation.Yaw, 0);

	const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	
	const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

	AddMovementInput(RightDirection, X);
	AddMovementInput(ForwardDirection, Z);
}

void ABCharacter::Look(float Yaw, float Pitch)
{
	AddControllerYawInput(Yaw);
	AddControllerPitchInput(Pitch);
}

void ABCharacter::Crouch(bool bClientSimulation)
{
	Super::Crouch(bClientSimulation);
	
	Cast<UCharacterAnimInstance>(GetMesh()->GetAnimInstance())->SetCrouch(true);
	FlexibleCameraBoom->ChangeArmOffsetToTemplate(FString("Crouch"));
}


void ABCharacter::UnCrouch(bool bClientSimulation)
{
	Super::UnCrouch(bClientSimulation);
	
	Cast<UCharacterAnimInstance>(GetMesh()->GetAnimInstance())->SetCrouch(false);
	FlexibleCameraBoom->ChangeArmOffsetToTemplate(FString("Default"));
}

void ABCharacter::SprintTrigger(bool IsTrigger)
{
	IsSprint = IsTrigger;
	GetCharacterMovement()->MaxWalkSpeed = GetSpeed();
}

void ABCharacter::AimTrigger(bool IsTrigger)
{
	IsAiming = IsTrigger;
	Cast<UCharacterAnimInstance>(GetMesh()->GetAnimInstance())->SetAiming(IsAiming);
	GetCharacterMovement()->MaxWalkSpeed = GetSpeed();
}

void ABCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}