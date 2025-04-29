// Fill out your copyright notice in the Description page of Project Settings.


#include "UnrealCharacterMoveComponent.h"

#include "Net/UnrealNetwork.h"
#include "Prepper/GamePlay/Character/BCharacter.h"


void UUnrealCharacterMoveComponent::OnRep_Crouching()
{
	if (IsLocal) return;
	CrouchingAct(IsCrouching);
}

void UUnrealCharacterMoveComponent::OnRep_Sprint()
{
	if (IsLocal) return;
	IsSprintLocal = IsSprint;
}

void UUnrealCharacterMoveComponent::OnRep_Jump()
{
	if (IsLocal) return;
	JumpAct(IsJump);
}

void UUnrealCharacterMoveComponent::OnRep_Aiming()
{
}

// Sets default values for this component's properties
UUnrealCharacterMoveComponent::UUnrealCharacterMoveComponent()
{
	PrimaryComponentTick.bCanEverTick = false;

	IsCrouching = false;
	IsSprint = false;
	IsJump = false;
	
	IsCrouchingLocal = false;
	IsAimingLocal = false;
	IsSprintLocal = false;

	IsLocal = false;
}

float UUnrealCharacterMoveComponent::GetSpeed() const
{
	if (IsAimingLocal) return AimMovementSpeed;
	if (IsSprintLocal) return SprintSpeed;
	return WalkSpeed;
}

void UUnrealCharacterMoveComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	DOREPLIFETIME(UUnrealCharacterMoveComponent, IsCrouching);
	DOREPLIFETIME(UUnrealCharacterMoveComponent, IsSprint);
	DOREPLIFETIME(UUnrealCharacterMoveComponent, IsJump);
}

void UUnrealCharacterMoveComponent::CrouchingAct(const bool IsTrigger) const
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

void UUnrealCharacterMoveComponent::JumpAct(const bool IsTrigger) const
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

void UUnrealCharacterMoveComponent::BeginPlay()
{
	Super::BeginPlay();
	
	if (!GetOwner()->GetInstigatorController()) return;
	
	if (GetOwner()->GetInstigatorController()->IsLocalController())
	{
		IsLocal = true;
	}
}

void UUnrealCharacterMoveComponent::CrouchToggle()
{
	IsCrouchingLocal = !IsCrouchingLocal;
	ServerCrouchTrigger(IsCrouchingLocal);
	CrouchingAct(IsCrouchingLocal);
}

void UUnrealCharacterMoveComponent::JumpTrigger(bool IsTrigger)
{
	ServerJumpTrigger(IsTrigger);
	JumpAct(IsTrigger);
}

void UUnrealCharacterMoveComponent::SprintTrigger(bool IsTrigger)
{
	IsSprintLocal = IsTrigger;
	ServerSprintTrigger(IsSprintLocal);
}

void UUnrealCharacterMoveComponent::SetAiming(bool IsTrigger)
{
	IsAimingLocal = IsTrigger;
}

void UUnrealCharacterMoveComponent::ServerCrouchTrigger_Implementation(bool IsTrigger)
{
	IsCrouching = IsTrigger;
}

void UUnrealCharacterMoveComponent::ServerSprintTrigger_Implementation(bool IsTrigger)
{
	IsSprint = IsTrigger;
}

void UUnrealCharacterMoveComponent::ServerJumpTrigger_Implementation(bool IsTrigger)
{
	IsJump = IsTrigger;
}
