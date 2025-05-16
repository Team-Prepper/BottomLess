// Fill out your copyright notice in the Description page of Project Settings.


#include "UnrealAmmoBoxComponent.h"

#include "Net/UnrealNetwork.h"
#include "Prepper/__Legacy/Character/Component/Combat/CombatComponent.h"


// Sets default values for this component's properties
UUnrealAmmoBoxComponent::UUnrealAmmoBoxComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	SetIsReplicated(true);
}

void UUnrealAmmoBoxComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(UUnrealAmmoBoxComponent, ReplicatedWeaponAmmoData);
}

void UUnrealAmmoBoxComponent::SetAmmoMap(const TArray<FWeaponConvertData>& AmmoMap)
{
	Super::SetAmmoMap(AmmoMap);
	ConvertMapToArray();
}

void UUnrealAmmoBoxComponent::AddAmmo(EWeaponType WeaponType, int32 AmmoAmount)
{
	Super::AddAmmo(WeaponType, AmmoAmount);
	ConvertMapToArray();
}

int UUnrealAmmoBoxComponent::UseAmmo(EWeaponType WeaponType, int32 MaxUse)
{
	const int Retval = Super::UseAmmo(WeaponType, MaxUse);
	ConvertMapToArray();
	
	return Retval;
}

void UUnrealAmmoBoxComponent::OnRep_Ammo()
{
	ConvertArrayToMap();
}

void UUnrealAmmoBoxComponent::ConvertArrayToMap()
{
	CarriedAmmoMap.Empty();
	for (const auto& Elem : ReplicatedWeaponAmmoData)
	{
		CarriedAmmoMap.Add(Elem.WeaponType, Elem.Count);
	}
}

void UUnrealAmmoBoxComponent::ConvertMapToArray()
{
	ReplicatedWeaponAmmoData.Empty();
	for (const auto& Elem : CarriedAmmoMap)
	{
		FWeaponConvertData ItemData;
		ItemData.WeaponType = Elem.Key;
		ItemData.Count = Elem.Value;
		ReplicatedWeaponAmmoData.Add(ItemData);
	}
}
