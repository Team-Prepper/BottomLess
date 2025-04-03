// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#define TRACE_LEN 80000.f

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "WeaponTargeting.generated.h"


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class PREPPER_API UWeaponTargeting : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UWeaponTargeting();
	virtual TArray<FVector_NetQuantize> GetTarget(FVector& HitTarget);
};
