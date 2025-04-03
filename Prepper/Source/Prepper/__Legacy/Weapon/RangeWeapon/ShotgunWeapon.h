// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "HitScanWeapon.h"
#include "ShotgunWeapon.generated.h"

/**
 * 
 */
UCLASS()
class PREPPER_API AShotgunWeapon : public AHitScanWeapon
{
	GENERATED_BODY()
public:
	virtual void Fire(const TArray<FVector_NetQuantize>& HitTargets) override;
	
private:

	UPROPERTY(EditAnywhere, Category = "Weapon Scatter")
	uint32 NumberOfPellets = 10;

	void ShotgunTraceEndWithScatter(const FVector& HitTarget, TArray<FVector_NetQuantize>& HitTargets);

};
