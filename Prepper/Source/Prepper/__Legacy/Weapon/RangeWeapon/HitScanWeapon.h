// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "RangeWeapon.h"
#include "HitScanWeapon.generated.h"

class UNiagaraSystem;
/**
 * 
 */
UCLASS()
class PREPPER_API AHitScanWeapon : public ARangeWeapon
{
	GENERATED_BODY()
	
public:
	virtual void Fire(const TArray<FVector_NetQuantize>& HitTargets) override;
	void FireEffect();
	void HitEffect(const FHitResult& FireHit);

protected:
	bool WeaponTraceHit(const FVector& TraceStart, const FVector& HitTarget, FHitResult& OutHit);

    UPROPERTY(EditAnywhere)
    UNiagaraSystem* ImpactParticles;

	UPROPERTY(EditAnywhere)
	USoundCue* HitSound;
	
private:
	UPROPERTY(EditAnywhere)
	UParticleSystem* BeamParticles;

	UPROPERTY(EditAnywhere)
	UNiagaraSystem* MuzzleFlash;

	UPROPERTY(EditAnywhere)
	USoundCue* FireSound;
};
