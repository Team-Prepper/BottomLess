// Fill out your copyright notice in the Description page of Project Settings.


#include "UnrealCharacterMoveComponent.h"

#include "Net/UnrealNetwork.h"
#include "Prepper/GamePlay/Character/BCharacter.h"

// Sets default values for this component's properties
UUnrealCharacterMoveComponent::UUnrealCharacterMoveComponent()
{
	PrimaryComponentTick.bCanEverTick = false;

	IsSprint = false;
	IsCrouching = false;
	IsJump = false;
	
	IsSprintLocal = false;
	IsAimingLocal = false;
	IsCrouchingLocal = false;

	IsLocal = false;

	SetIsReplicated(true);
}

void UUnrealCharacterMoveComponent::OnRep_Sprint()
{
	if (IsLocal) return;
	IsSprintLocal = IsSprint;
	SetOwnerSpeed();
}

void UUnrealCharacterMoveComponent::OnRep_Aiming()
{
	if (IsLocal) return;
	IsAimingLocal = IsAiming;
	SetOwnerSpeed();
}

void UUnrealCharacterMoveComponent::OnRep_Crouching()
{
	if (IsLocal) return;
	IsCrouchingLocal = IsCrouching;
	CrouchingAct(IsCrouchingLocal);
}

void UUnrealCharacterMoveComponent::OnRep_Jump()
{
	if (IsLocal) return;
	JumpAct(IsJump);
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

void UUnrealCharacterMoveComponent::SetOwnerSpeed()
{
	GetOwner<ABCharacter>()->SetMaxSpeed(GetSpeed());
}

void UUnrealCharacterMoveComponent::BeginPlay()
{
	Super::BeginPlay();
	
	if (!GetOwner()->GetInstigatorController()) return;
	
	IsLocal = GetOwner()->GetInstigatorController()->IsLocalController();
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
	SetOwnerSpeed();
}

void UUnrealCharacterMoveComponent::SetAiming(bool IsTrigger)
{
	IsAimingLocal = IsTrigger;
	ServerAimingTrigger(IsAimingLocal);
	SetOwnerSpeed();
}

void UUnrealCharacterMoveComponent::ServerSprintTrigger_Implementation(bool IsTrigger)
{
	IsSprint = IsTrigger;
	IsSprintLocal = IsSprint;
	SetOwnerSpeed();
}

void UUnrealCharacterMoveComponent::ServerAimingTrigger_Implementation(bool IsTrigger)
{
	IsAiming = IsTrigger;
	IsAimingLocal = IsAiming;
	SetOwnerSpeed();
}

void UUnrealCharacterMoveComponent::ServerCrouchTrigger_Implementation(bool IsTrigger)
{
	IsCrouching = IsTrigger;
	IsCrouchingLocal = IsCrouching;
	CrouchingAct(IsCrouchingLocal);
}

void UUnrealCharacterMoveComponent::ServerJumpTrigger_Implementation(bool IsTrigger)
{
	IsJump = IsTrigger;
	JumpAct(IsJump);
}
