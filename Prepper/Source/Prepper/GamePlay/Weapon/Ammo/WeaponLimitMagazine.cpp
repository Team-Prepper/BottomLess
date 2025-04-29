// Fill out your copyright notice in the Description page of Project Settings.


#include "WeaponLimitMagazine.h"

#include "Prepper/GamePlay/Weapon/AmmoBox.h"
#include "Prepper/GamePlay/Weapon/WeaponTypes.h"

UWeaponLimitMagazine::UWeaponLimitMagazine()
{
	MaxAmmo = 20;
	CurAmmo = 20;
}

int UWeaponLimitMagazine::GetLeftAmmo()
{
	return CurAmmo;
}

bool UWeaponLimitMagazine::CanAttack()
{
	return CurAmmo > 0;
}

bool UWeaponLimitMagazine::CanReload()
{
	return CurAmmo < MaxAmmo;
}

void UWeaponLimitMagazine::Reload(IAmmoBox* AmmoBox, EWeaponType WeaponType)
{
	const int AddAmount = AmmoBox->UseAmmo(WeaponType, MaxAmmo - CurAmmo);
	UE_LOG(LogTemp, Warning, TEXT("Ammo: %d"), AddAmount);
	CurAmmo += AddAmount;
}

FString UWeaponLimitMagazine::ToString()
{
	return FString::Printf(TEXT("%d / %d"), CurAmmo, MaxAmmo);
}

void UWeaponLimitMagazine::UseAmmo(const int Amount)
{
	CurAmmo -= Amount;
}

void UWeaponLimitMagazine::SetAmmo(int Amount)
{
	CurAmmo = Amount;
}
