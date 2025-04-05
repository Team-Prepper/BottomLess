// Fill out your copyright notice in the Description page of Project Settings.


#include "WeaponProjectileAttacking.h"

#include "Prepper/__Legacy/Weapon/Projectile/Projectile.h"


// Sets default values for this component's properties
UWeaponProjectileAttacking::UWeaponProjectileAttacking()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UWeaponProjectileAttacking::Fire(FVector Muzzle,
	const TArray<FVector_NetQuantize>& HitTargets, AController* Attacker, bool IsSimulate)
{
	if (IsSimulate) return;
	if (!ProjectileClass) return;
	
	UWorld* World = GetWorld();
	
	if (!World) return;

	for(FVector HitTarget : HitTargets)
	{
		FVector ToTarget = HitTarget - Muzzle;
		FRotator TargetRotation = ToTarget.Rotation();
		FActorSpawnParameters SpawnParams;
		SpawnParams.Owner = GetOwner()->GetOwner();
	
		World->SpawnActor<AProjectile>(
			ProjectileClass,
			Muzzle,
			TargetRotation,
			SpawnParams
			);
		
	}
	
}