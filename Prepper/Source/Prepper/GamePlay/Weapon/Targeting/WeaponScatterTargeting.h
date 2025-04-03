// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "WeaponTargeting.h"
#include "WeaponScatterTargeting.generated.h"


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class PREPPER_API UWeaponScatterTargeting : public UWeaponTargeting
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, Category="Targeting")
	FTransform Socket;
	UPROPERTY(EditAnywhere, Category="Targeting")
	float DistanceToSphere = 800.f;
	UPROPERTY(EditAnywhere, Category="Targeting")
	float SphereRadius = 75.f;
	UPROPERTY(EditAnywhere, Category = "Targeting")
	uint32 NumberOfPellets = 10;
	
public:
	// Sets default values for this component's properties
	UWeaponScatterTargeting();
	virtual TArray<FVector_NetQuantize> GetTarget(FVector& HitTarget) override;
};
