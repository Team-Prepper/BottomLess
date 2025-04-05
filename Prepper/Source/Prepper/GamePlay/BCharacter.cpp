// Fill out your copyright notice in the Description page of Project Settings.


#include "BCharacter.h"

#include "GameFramework/CharacterMovementComponent.h"
#include "Prepper/__Legacy/Component/CustomCameraComponent.h"
#include "Prepper/__Legacy/Component/FlexibleSpringArmComponent/FlexibleSpringArmComponent.h"


float ABCharacter::GetSpeed() const
{
	if (IsAiming) return AimMovementSpeed;
	if (IsSprint) return SprintSpeed;
	return WalkSpeed;
}

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
	
}

void ABCharacter::GetLookDirection(FVector& Start, FVector& Forward)
{
	Start = FollowCamera->GetComponentLocation();
	Forward = FollowCamera->GetForwardVector();
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
	FlexibleCameraBoom->ChangeArmOffsetToTemplate(FString("Crouch"));
}


void ABCharacter::UnCrouch(bool bClientSimulation)
{
	Super::UnCrouch(bClientSimulation);

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
	GetCharacterMovement()->MaxWalkSpeed = GetSpeed();
}

void ABCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}