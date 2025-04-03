// Fill out your copyright notice in the Description page of Project Settings.


#include "WeaponAttacking.h"


// Sets default values for this component's properties
UWeaponAttacking::UWeaponAttacking()
{
	PrimaryComponentTick.bCanEverTick = false;
	
}

TArray<FVector_NetQuantize> UWeaponAttacking::Fire(FVector Muzzle, const TArray<FVector_NetQuantize>& HitTargets, AController* Attacker)
{
	return TArray<FVector_NetQuantize>();
}

void UWeaponAttacking::HitEffect(const TArray<FVector_NetQuantize>& HitTargets)
{
	
}
