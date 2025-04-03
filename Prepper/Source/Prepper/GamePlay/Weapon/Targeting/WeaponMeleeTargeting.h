// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "WeaponTargeting.h"
#include "WeaponMeleeTargeting.generated.h"


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class PREPPER_API UWeaponMeleeTargeting : public UWeaponTargeting
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, Category ="Targeting")
	FVector MeleeTypeVector = FVector(0.f, 0.f, 0.f);
public:
	// Sets default values for this component's properties
	UWeaponMeleeTargeting();
	virtual TArray<FVector_NetQuantize> GetTarget(FVector& HitTarget) override;
};
