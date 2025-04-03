// Fill out your copyright notice in the Description page of Project Settings.

#include "UnrealCombatComponent.h"

#include "Net/UnrealNetwork.h"
#include "Prepper/GamePlay/BCharacter.h"
#include "Prepper/GamePlay/CharacterController/CharacterController.h"

class ICharacterController;
// Sets default values for this component's properties
UUnrealCombatComponent::UUnrealCombatComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

	EquippedWeapon = nullptr;
	
	IsAiming = false;
	IsAttack = false;
	
	IsAimingLocal = false;
	IsAttackLocal = false;
}

// Called when the game starts
void UUnrealCombatComponent::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void UUnrealCombatComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void UUnrealCombatComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(UUnrealCombatComponent, IsAiming);
	DOREPLIFETIME(UUnrealCombatComponent, IsAttack);
}

void UUnrealCombatComponent::OnRep_Aiming()
{
	if (GetOwner<APlayerController>()->IsLocalController()) return;
	AimingAct(IsAiming);
}

void UUnrealCombatComponent::OnRep_Attack()
{
	if (GetOwner<APlayerController>()->IsLocalController()) return;
	AttackAct(IsAttack);
}

void UUnrealCombatComponent::OnRep_EquippedWeapon()
{
	if (!EquippedWeapon) return;
	
}

void UUnrealCombatComponent::AimingAct(bool IsTrigger)
{
	const TObjectPtr<ABCharacter> Target = GetOwner<ICharacterController>()->GetTargetCharacter();
	if (Target == nullptr) return;
	Target->AimTrigger(IsTrigger);
}

void UUnrealCombatComponent::AttackAct(bool IsTrigger)
{
	//TargetCharacter->AttackTrigger(IsTrigger);
}

void UUnrealCombatComponent::AimTrigger(bool IsTrigger)
{
	IsAimingLocal = IsTrigger;
	ServerAimTrigger(IsAimingLocal);
	AimingAct(IsAimingLocal);
}

void UUnrealCombatComponent::AttackTrigger(bool IsTrigger)
{
	IsAttackLocal = IsTrigger;
	ServerAttackTrigger(IsAttackLocal);
	AttackAct(IsAttackLocal);
}

void UUnrealCombatComponent::Reload()
{
	ServerReload();
}

void UUnrealCombatComponent::ServerAimTrigger_Implementation(bool IsTrigger)
{
	IsAiming = IsTrigger;
}

void UUnrealCombatComponent::ServerAttackTrigger_Implementation(bool IsTrigger)
{
	IsAttack = IsTrigger;
}

void UUnrealCombatComponent::ServerReload_Implementation()
{
	
}