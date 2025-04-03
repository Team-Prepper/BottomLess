// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "WeaponAttacking.h"
#include "WeaponProjectileAttacking.generated.h"


class AProjectile;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class PREPPER_API UWeaponProjectileAttacking : public UWeaponAttacking
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, Category="Attack")
	TSubclassOf<AProjectile> ProjectileClass;
	
public:
	// Sets default values for this component's properties
	UWeaponProjectileAttacking();

protected:
	// Called when the game starts
	virtual TArray<FVector_NetQuantize> Fire(FVector Muzzle, const TArray<FVector_NetQuantize>& HitTargets, AController* Attacker) override;
};
