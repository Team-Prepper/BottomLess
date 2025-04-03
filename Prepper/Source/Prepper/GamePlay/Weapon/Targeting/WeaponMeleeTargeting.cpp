// Fill out your copyright notice in the Description page of Project Settings.


#include "WeaponMeleeTargeting.h"


// Sets default values for this component's properties
UWeaponMeleeTargeting::UWeaponMeleeTargeting()
{
	PrimaryComponentTick.bCanEverTick = false;
	MeleeTypeVector = FVector(0.f, 0.f, 0.f);
}

TArray<FVector_NetQuantize> UWeaponMeleeTargeting::GetTarget(FVector& HitTarget)
{
	return Super::GetTarget(MeleeTypeVector);
}
