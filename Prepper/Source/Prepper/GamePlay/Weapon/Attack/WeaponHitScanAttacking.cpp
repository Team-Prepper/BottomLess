// Fill out your copyright notice in the Description page of Project Settings.


#include "WeaponHitScanAttacking.h"

#include "Prepper/__Legacy/Interfaces/Damageable.h"

// Sets default values for this component's properties
UWeaponHitScanAttacking::UWeaponHitScanAttacking()
{
	PrimaryComponentTick.bCanEverTick = false;
	
	Damage = 30.f;
}

TArray<FVector_NetQuantize> UWeaponHitScanAttacking::Fire(FVector Muzzle, const TArray<FVector_NetQuantize>& HitTargets, AController* Attacker)
{
	TArray<FVector_NetQuantize> Retval;

	TMap<IDamageable*, uint32> HitMap;
	
	UE_LOG(LogTemp, Warning, TEXT("타겟 수 : %d"), HitTargets.Num());
	
	for(FVector_NetQuantize HitTarget : HitTargets)
	{
		FHitResult FireHit;
		
		WeaponTraceHit(Muzzle, HitTarget, FireHit);
		Retval.Add(FireHit.Location);
		
		IDamageable* DamagedTarget = Cast<IDamageable>(FireHit.GetActor());
		
		if (!DamagedTarget) continue;

		if (HitMap.Contains(DamagedTarget))
		{
			HitMap[DamagedTarget]++;
			continue;
		}
		HitMap.Emplace(DamagedTarget, 1);
	}
	
	for (auto HitPair : HitMap)
	{
		if (!HitPair.Key) continue;

		UE_LOG(LogTemp, Warning, TEXT("데미지: %d"), HitPair.Value);
		HitPair.Key->ReceiveDamage(Damage * HitPair.Value, Attacker, GetOwner());
		
	}
	
	return Retval;
}

bool UWeaponHitScanAttacking::WeaponTraceHit(const FVector& TraceStart, const FVector& HitTarget, FHitResult& OutHit) const
{
	const TObjectPtr<UWorld> World = GetWorld();
	if (!World) return false;
	
	bool IsBlock = false;
	FVector End = TraceStart + (HitTarget - TraceStart) * 1.25f;

	World->LineTraceSingleByChannel(
		OutHit,
		TraceStart,
		End,
		ECollisionChannel::ECC_Visibility
	);
	if (OutHit.bBlockingHit)
	{
		End = OutHit.ImpactPoint;
		IsBlock = true;
	}
	return IsBlock;
}
