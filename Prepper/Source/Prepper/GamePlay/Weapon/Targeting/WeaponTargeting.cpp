// Fill out your copyright notice in the Description page of Project Settings.


#include "WeaponTargeting.h"


// Sets default values for this component's properties
UWeaponTargeting::UWeaponTargeting()
{
	PrimaryComponentTick.bCanEverTick = false;
}

TArray<FVector_NetQuantize> UWeaponTargeting::GetTarget(FVector& HitTarget)
{
	TArray<FVector_NetQuantize> HitTargets;
	HitTargets.Add(HitTarget);
	return HitTargets;
}