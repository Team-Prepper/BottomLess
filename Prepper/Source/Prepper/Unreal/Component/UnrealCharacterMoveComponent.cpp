// Fill out your copyright notice in the Description page of Project Settings.


#include "UnrealCharacterMoveComponent.h"

#include "Net/UnrealNetwork.h"
#include "Prepper/GamePlay/Character/BCharacter.h"

// Sets default values for this component's properties
UUnrealCharacterMoveComponent::UUnrealCharacterMoveComponent()
{
	PrimaryComponentTick.bCanEverTick = false;

	IsSprintNetwork = false;
	IsCrouchingNetwork = false;
	IsJumpNetwork = false;
	
	IsSprintLocal = false;
	IsAimingLocal = false;
	IsCrouchingLocal = false;

	IsLocal = false;

	SetIsReplicated(true);
}

void UUnrealCharacterMoveComponent::OnRep_Sprint()
{
	if (IsLocal) return;
	IsSprintLocal = IsSprintNetwork;
	SetOwnerSpeed();
}

void UUnrealCharacterMoveComponent::OnRep_Aiming()
{
	if (IsLocal) return;
	IsAimingLocal = IsAimingNetwork;
	SetOwnerSpeed();
}

void UUnrealCharacterMoveComponent::OnRep_Crouching()
{
	if (IsLocal) return;
	IsCrouchingLocal = IsCrouchingNetwork;
	CrouchingAct(IsCrouchingLocal);
}

void UUnrealCharacterMoveComponent::OnRep_Jump()
{
	if (IsLocal) return;
	JumpAct(IsJumpNetwork);
}

float UUnrealCharacterMoveComponent::GetSpeed() const
{
	if (IsAimingLocal) return AimMovementSpeed * CoefficientMovementSpeed;
	if (IsSprintLocal) return SprintSpeed * CoefficientMovementSpeed;
	return WalkSpeed * CoefficientMovementSpeed;
}

void UUnrealCharacterMoveComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	DOREPLIFETIME(UUnrealCharacterMoveComponent, IsCrouchingNetwork);
	DOREPLIFETIME(UUnrealCharacterMoveComponent, IsSprintNetwork);
	DOREPLIFETIME(UUnrealCharacterMoveComponent, IsJumpNetwork);
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
	IsSprintNetwork = IsTrigger;
	IsSprintLocal = IsSprintNetwork;
	SetOwnerSpeed();
}

void UUnrealCharacterMoveComponent::ServerAimingTrigger_Implementation(bool IsTrigger)
{
	IsAimingNetwork = IsTrigger;
	IsAimingLocal = IsAimingNetwork;
	SetOwnerSpeed();
}

void UUnrealCharacterMoveComponent::ServerCrouchTrigger_Implementation(bool IsTrigger)
{
	IsCrouchingNetwork = IsTrigger;
	IsCrouchingLocal = IsCrouchingNetwork;
	CrouchingAct(IsCrouchingLocal);
}

void UUnrealCharacterMoveComponent::ServerJumpTrigger_Implementation(bool IsTrigger)
{
	IsJumpNetwork = IsTrigger;
	JumpAct(IsJumpNetwork);
}
