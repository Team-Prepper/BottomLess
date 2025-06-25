// Fill out your copyright notice in the Description page of Project Settings.


#include "CharacterMoveComponent.h"

#include "Prepper/GamePlay/Car/Car.h"
#include "Prepper/GamePlay/Character/BCharacter.h"


void UCharacterMoveComponent::CrouchingAct(bool IsTrigger) const
{
	const TObjectPtr<ABCharacter> Target = GetOwner<ABCharacter>();
	if (Target == nullptr) return;
	if (IsTrigger)
	{
		Target->Crouch();
		return;
	}
	Target->UnCrouch();
}

void UCharacterMoveComponent::JumpAct(const bool IsTrigger) const
{
	const TObjectPtr<ABCharacter> Target = GetOwner<ABCharacter>();
	if (Target == nullptr) return;
	if (IsTrigger)
	{
		Target->Jump();
		return;
	}
	Target->StopJumping();
}

void UCharacterMoveComponent::SetOwnerSpeed() const
{
	GetOwner<ABCharacter>()->SetMaxSpeed(GetSpeed());
}

// Sets default values for this component's properties
UCharacterMoveComponent::UCharacterMoveComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;
	
	IsSprintLocal = false;
	IsAimingLocal = false;
	IsCrouchingLocal = false;
}

float UCharacterMoveComponent::GetSpeed() const
{
	if (TargetCar != nullptr) return 0;
	if (IsAimingLocal) return AimMovementSpeed * CoefficientMovementSpeed;
	if (IsSprintLocal) return SprintSpeed * CoefficientMovementSpeed;
	return WalkSpeed * CoefficientMovementSpeed;
}

void UCharacterMoveComponent::CrouchToggle()
{
	IsCrouchingLocal = !IsCrouchingLocal;
}

void UCharacterMoveComponent::JumpTrigger(bool IsTrigger)
{
	if (IsCrouchingLocal)
	{
		CrouchToggle();
		return;
	}
	JumpAct(IsTrigger);
}

void UCharacterMoveComponent::SprintTrigger(bool IsTrigger)
{
	IsSprintLocal = IsTrigger;
	SetOwnerSpeed();
}

void UCharacterMoveComponent::SetAiming(bool IsTrigger)
{
	IsAimingLocal = IsTrigger;
	SetOwnerSpeed();
}

void UCharacterMoveComponent::SetCar(const TObjectPtr<ACar> Vehicle)
{
	TargetCar = Vehicle;

	SetOwnerSpeed();
	
	if (TargetCar == nullptr)
	{
		GetOwner()->SetActorEnableCollision(true);
		GetOwner()->SetActorHiddenInGame(false);
		return;
	}
	
	GetOwner()->SetActorEnableCollision(false);
	GetOwner()->SetActorHiddenInGame(true);

	const TObjectPtr<APlayerController> Controller = GetOwner<APawn>()->GetController<APlayerController>();

	if (Controller == nullptr) return;
	
	Controller->SetViewTarget(Vehicle);
	
}
