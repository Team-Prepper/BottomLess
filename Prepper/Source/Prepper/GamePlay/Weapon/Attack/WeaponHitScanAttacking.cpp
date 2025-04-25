// Fill out your copyright notice in the Description page of Project Settings.


#include "WeaponHitScanAttacking.h"

#include "NiagaraFunctionLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "Prepper/__Legacy/Interfaces/Damageable.h"

// Sets default values for this component's properties
UWeaponHitScanAttacking::UWeaponHitScanAttacking()
{
	PrimaryComponentTick.bCanEverTick = false;
	
	Damage = 30.f;
	ImpactParticles = nullptr;
	HitSound = nullptr;
}

void UWeaponHitScanAttacking::Fire(FVector Muzzle,
                                   const TArray<FVector_NetQuantize>& HitTargets, AController* Attacker, bool IsSimulate)
{
	const TObjectPtr<UWorld> World = GetWorld();

	if (World == nullptr) return;
	
	TArray<FHitResult> HitResults;
	
	for(FVector_NetQuantize HitTarget : HitTargets)
	{
		FVector TraceEnd = Muzzle + (HitTarget - Muzzle) * 1.25f;
		
		if (FHitResult FireHit; WeaponTraceHit(World, Muzzle, TraceEnd, FireHit))
		{
			HitResults.Add(FireHit);
		}
		BeamEffect(World, Muzzle, TraceEnd);
	}
	
	for (FHitResult HitResult : HitResults)
	{
		HitEffect(World, HitResult);
	}
	
	if (IsSimulate) return;

	TMap<IDamageable*, uint32> HitMap;
	
	for (FHitResult HitResult : HitResults)
	{
		IDamageable* DamagedTarget = Cast<IDamageable>(HitResult.GetActor());
		
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

		HitPair.Key->ReceiveDamage(Damage * HitPair.Value, Attacker, GetOwner());
		
	}
	
}

bool UWeaponHitScanAttacking::WeaponTraceHit(const TObjectPtr<UWorld> World,
	const FVector& TraceStart, FVector& TraceEnd, FHitResult& OutHit) const
{
	if (World == nullptr) return false;
	
	bool IsBlock = false;

	World->LineTraceSingleByChannel(
		OutHit, TraceStart, TraceEnd,
		ECC_Visibility
	);
	if (OutHit.bBlockingHit)
	{
		TraceEnd = OutHit.ImpactPoint;
		IsBlock = true;
	}
	return IsBlock;
}


void UWeaponHitScanAttacking::BeamEffect(const TObjectPtr<UWorld> World, const FVector& TraceStart, const FVector& TraceEnd) const
{
	if (World == nullptr) return;
	if (BeamParticles == nullptr) return;
	
	UParticleSystemComponent* Beam = UGameplayStatics::SpawnEmitterAtLocation(
		World,
		BeamParticles,
		TraceStart,
		FRotator::ZeroRotator,
		true
	);
	
	if (Beam == nullptr) return;
	
	Beam->SetVectorParameter(FName("Target"), TraceEnd);
}


void UWeaponHitScanAttacking::HitEffect(const TObjectPtr<UWorld> World, const FHitResult& FireHit) const
{
	if(ImpactParticles)
	{
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(
			World,ImpactParticles,
			FireHit.ImpactPoint, FireHit.ImpactNormal.Rotation()
		);
	}
	if(HitSound == nullptr) return;
	
	UGameplayStatics::PlaySoundAtLocation(
	this, HitSound, FireHit.ImpactPoint);
	
}