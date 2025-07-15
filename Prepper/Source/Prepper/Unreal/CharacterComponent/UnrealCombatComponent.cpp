// Fill out your copyright notice in the Description page of Project Settings.

#include "UnrealCombatComponent.h"

#include "Net/UnrealNetwork.h"
#include "Prepper/GamePlay/Character/BCharacter.h"
#include "Prepper/GamePlay/Weapon/Weapon.h"

// Sets default values for this component's properties
UUnrealCombatComponent::UUnrealCombatComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	SetIsReplicated(true);
	
	NetworkEquippedWeapon = nullptr;
	NetworkSecondaryWeapon = nullptr;
	NetworkDroppedWeapon = nullptr;
	
	NetworkEquippedAmmo = 0;
	NetworkIsAiming = false;
}

void UUnrealCombatComponent::Swap()
{
	Super::Swap();
	
	NetworkEquippedWeapon = EquippedWeapon;
	NetworkSecondaryWeapon = SecondaryWeapon;
}

void UUnrealCombatComponent::CharacterElim()
{
	MulticastCharacterElim();
}

void UUnrealCombatComponent::MulticastCharacterElim_Implementation()
{
	Super::CharacterElim();
}

void UUnrealCombatComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(UUnrealCombatComponent, NetworkIsAiming);
	DOREPLIFETIME(UUnrealCombatComponent, NetworkEquippedWeapon);
	DOREPLIFETIME(UUnrealCombatComponent, NetworkEquippedAmmo);
	DOREPLIFETIME(UUnrealCombatComponent, NetworkSecondaryWeapon);
	DOREPLIFETIME(UUnrealCombatComponent, NetworkDroppedWeapon);
}

void UUnrealCombatComponent::EquipWeapon(TObjectPtr<ABCharacter> Target, TObjectPtr<AWeapon> Weapon)
{
	const TObjectPtr<AWeapon> BeforeEquippedWeapon = EquippedWeapon;
	
	Super::EquipWeapon(Target, Weapon);

	if (BeforeEquippedWeapon != EquippedWeapon)
	{
		NetworkDroppedWeapon = BeforeEquippedWeapon;
	}
	else
	{
		NetworkDroppedWeapon = nullptr;
	}
	
	NetworkEquippedWeapon = EquippedWeapon;
	NetworkSecondaryWeapon = SecondaryWeapon;
	
}

void UUnrealCombatComponent::OnRep_Aiming()
{
	if (GetOwner()->GetInstigatorController() &&
		GetOwner()->GetInstigatorController()->IsLocalController()) return;
	AimingAct(NetworkIsAiming);
}

void UUnrealCombatComponent::OnRep_Ammo()
{
	Super::SetEquippedAmmo(NetworkEquippedAmmo);
}

void UUnrealCombatComponent::OnRep_EquippedWeapon()
{
	EquippedAct(NetworkEquippedWeapon);
}

void UUnrealCombatComponent::OnRep_SecondaryWeapon()
{
	SecondaryEquippedAct(NetworkSecondaryWeapon);
}

void UUnrealCombatComponent::OnRep_DroppedWeapon()
{
	if (NetworkDroppedWeapon == nullptr) return;
	NetworkDroppedWeapon->OnDropped();
}

void UUnrealCombatComponent::FireWeaponToTargets(const TArray<FVector_NetQuantize>& TraceHitTargets) const
{
	MulticastFireWeaponToTargets(TraceHitTargets);
}

void UUnrealCombatComponent::MulticastFireWeaponToTargets_Implementation(
	const TArray<FVector_NetQuantize>& TraceHitTargets) const
{
	Super::FireWeaponToTargets(TraceHitTargets);
}

void UUnrealCombatComponent::ReloadAct()
{
	MulticastReloadWeapon();
}

void UUnrealCombatComponent::MulticastReloadWeapon_Implementation()
{
	Super::ReloadAct();
}

void UUnrealCombatComponent::SetEquippedAmmo(const int AmmoCnt)
{
	NetworkEquippedAmmo = AmmoCnt;
	Super::SetEquippedAmmo(AmmoCnt);
}

void UUnrealCombatComponent::AimTrigger(const bool IsTrigger)
{
	IsAimingLocal = IsTrigger;
	ServerAimTrigger(IsAimingLocal);
	AimingAct(IsAimingLocal);
}

void UUnrealCombatComponent::AttackTrigger(const bool IsTrigger)
{
	IsAttack = IsTrigger;
	ServerAttackTrigger(IsAttack);
}

void UUnrealCombatComponent::Reload()
{
	ServerReload();
}

FString UUnrealCombatComponent::GetAmmoValue() const
{
	if (EquippedWeapon == nullptr) return FString("- / -");
	return EquippedWeapon->GetAmmoValue();
}

void UUnrealCombatComponent::ServerAimTrigger_Implementation(const bool IsTrigger)
{
	NetworkIsAiming = IsTrigger;
}

void UUnrealCombatComponent::ServerAttackTrigger_Implementation(const bool IsTrigger)
{
	Super::AttackTrigger(IsTrigger);
}

void UUnrealCombatComponent::ServerReload_Implementation()
{
	Super::Reload();
}