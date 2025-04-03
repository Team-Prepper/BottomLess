// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "RangeWeapon.h"
#include "ProjectileWeapon.generated.h"

class AProjectile;

UCLASS()
class PREPPER_API AProjectileWeapon : public ARangeWeapon
{
	GENERATED_BODY()
	
public:
	virtual void Fire(const TArray<FVector_NetQuantize>& HitTargets) override;

private:
	UPROPERTY(EditAnywhere, Category="Weapon Properties")
	TSubclassOf<AProjectile> ProjectileClass;
	
};
