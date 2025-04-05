// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "WeaponAttacking.h"
#include "WeaponHitScanAttacking.generated.h"


class UNiagaraSystem;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class PREPPER_API UWeaponHitScanAttacking : public UWeaponAttacking
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, Category="Attack")
	float Damage;
	
	UPROPERTY(EditAnywhere, Category="Attack")
	TObjectPtr<UNiagaraSystem> ImpactParticles;
	UPROPERTY(EditAnywhere, Category="Attack")
	TObjectPtr<UParticleSystem> BeamParticles;
	
	UPROPERTY(EditAnywhere, Category="Attack")
	USoundBase* HitSound;
	
public:
	UWeaponHitScanAttacking();
	virtual void Fire (FVector Muzzle,
		const TArray<FVector_NetQuantize>& HitTargets, AController* Attacker, bool IsSimulate) override;

private:
	bool WeaponTraceHit(TObjectPtr<UWorld> World, const FVector& TraceStart, FVector& TraceEnd, FHitResult& OutHit) const;
	void BeamEffect(TObjectPtr<UWorld> World, const FVector& TraceStart, const FVector& TraceEnd) const;
	void HitEffect(const TObjectPtr<UWorld> World, const FHitResult& FireHit) const;
};
