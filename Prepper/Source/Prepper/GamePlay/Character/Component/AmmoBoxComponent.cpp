// Fill out your copyright notice in the Description page of Project Settings.


#include "AmmoBoxComponent.h"


// Sets default values for this component's properties
UAmmoBoxComponent::UAmmoBoxComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UAmmoBoxComponent::AddAmmo(EWeaponType WeaponType, int32 AmmoAmount)
{
	if (!CarriedAmmoMap.Contains(WeaponType))
	{
		CarriedAmmoMap.Add(WeaponType, AmmoAmount);
		return;
	}
	
	CarriedAmmoMap[WeaponType] = FMath::Clamp(CarriedAmmoMap[WeaponType] + AmmoAmount, 0, MaxCarriedAmmo);
	
}

int UAmmoBoxComponent::UseAmmo(EWeaponType WeaponType, int32 MaxUse)
{
	if (!CarriedAmmoMap.Contains(WeaponType))
	{
		return 0;
	}
	
	int Retval = CarriedAmmoMap[WeaponType];
	if (Retval > MaxUse) Retval = MaxUse;

	CarriedAmmoMap[WeaponType] -= Retval;
	
	return Retval;
}
