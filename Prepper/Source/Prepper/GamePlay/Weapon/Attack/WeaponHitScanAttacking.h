// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "WeaponAttacking.h"
#include "WeaponHitScanAttacking.generated.h"


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class PREPPER_API UWeaponHitScanAttacking : public UWeaponAttacking
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, Category="Attack")
	float Damage;
	
public:
	UWeaponHitScanAttacking();
	virtual TArray<FVector_NetQuantize> Fire(FVector Muzzle, const TArray<FVector_NetQuantize>& HitTargets, AController* Attacker) override;

private:
	bool WeaponTraceHit(const FVector& TraceStart, const FVector& HitTarget, FHitResult& OutHit) const;
};
