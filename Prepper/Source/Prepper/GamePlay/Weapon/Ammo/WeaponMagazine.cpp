// Fill out your copyright notice in the Description page of Project Settings.


#include "WeaponMagazine.h"


// Sets default values for this component's properties
UWeaponMagazine::UWeaponMagazine()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	// ...
}

int UWeaponMagazine::GetLeftAmmo()
{
	return 0;
}

bool UWeaponMagazine::CanAttack()
{
	return true;
}

bool UWeaponMagazine::CanReload()
{
	return false;
}

void UWeaponMagazine::Reload(IAmmoBox* AmmoBox, EWeaponType WeaponType)
{
}

void UWeaponMagazine::UseAmmo(int Amount)
{
}

void UWeaponMagazine::SetAmmo(int Amount)
{
	
}

FString UWeaponMagazine::ToString()
{
	return FString(TEXT("- / -"));
}
