// Fill out your copyright notice in the Description page of Project Settings.


#include "UnrealCharacterMoveComponent.h"

#include "Net/UnrealNetwork.h"

// Sets default values for this component's properties
UUnrealCharacterMoveComponent::UUnrealCharacterMoveComponent()
{
	PrimaryComponentTick.bCanEverTick = false;

	IsSprintNetwork = false;
	IsCrouchingNetwork = false;
	IsJumpNetwork = false;

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

void UUnrealCharacterMoveComponent::OnRep_Car()
{
	Super::SetCar(TargetCarNetwork);
}

void UUnrealCharacterMoveComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	DOREPLIFETIME(UUnrealCharacterMoveComponent, IsCrouchingNetwork);
	DOREPLIFETIME(UUnrealCharacterMoveComponent, IsSprintNetwork);
	DOREPLIFETIME(UUnrealCharacterMoveComponent, IsJumpNetwork);
	DOREPLIFETIME(UUnrealCharacterMoveComponent, TargetCarNetwork);
}

void UUnrealCharacterMoveComponent::BeginPlay()
{
	Super::BeginPlay();
	
	if (!GetOwner()->GetInstigatorController()) return;
	
	IsLocal = GetOwner()->GetInstigatorController()->IsLocalController();
}

void UUnrealCharacterMoveComponent::CrouchToggle()
{
	Super::CrouchToggle();
	ServerCrouchTrigger(IsCrouchingLocal);
	CrouchingAct(IsCrouchingLocal);
}

void UUnrealCharacterMoveComponent::JumpTrigger(bool IsTrigger)
{
	Super::JumpTrigger(IsTrigger);
	ServerJumpTrigger(IsTrigger);
}

void UUnrealCharacterMoveComponent::SprintTrigger(bool IsTrigger)
{
	Super::SprintTrigger(IsTrigger);
	ServerSprintTrigger(IsSprintLocal);
}

void UUnrealCharacterMoveComponent::SetAiming(bool IsTrigger)
{
	Super::SetAiming(IsTrigger);
	ServerAimingTrigger(IsAimingLocal);
}

void UUnrealCharacterMoveComponent::SetCar(TObjectPtr<ACar> Vehicle)
{
	TargetCarNetwork = Vehicle;
	if (GetOwner()->HasAuthority()) return;
	Super::SetCar(Vehicle);
	return;
	ServerSetCar(Vehicle);
}

void UUnrealCharacterMoveComponent::ServerSetCar_Implementation(ACar* Vehicle)
{
	TargetCarNetwork = Vehicle;
	Super::SetCar(Vehicle);
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