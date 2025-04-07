// Fill out your copyright notice in the Description page of Project Settings.


#include "WeaponLimitMagazine.h"

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
	return CurAmmo != MaxAmmo;
}

void UWeaponLimitMagazine::Reload(const int Amount)
{
	CurAmmo += Amount;
}

FString UWeaponLimitMagazine::ToString()
{
	return Super::ToString();
}

void UWeaponLimitMagazine::UseAmmo(const int Amount)
{
	CurAmmo -= Amount;
}