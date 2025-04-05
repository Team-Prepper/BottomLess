// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "WeaponAttacking.generated.h"


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class PREPPER_API UWeaponAttacking : public UActorComponent
{
	GENERATED_BODY()

public:
	UWeaponAttacking();
	virtual void Fire (FVector Muzzle,
		const TArray<FVector_NetQuantize>& HitTargets, AController* Attacker, bool IsSimulate);
	
};