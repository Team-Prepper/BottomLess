// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Prepper/GamePlay/Character/Component/AmmoBoxComponent.h"
#include "UnrealAIAmmoBoxComponent.generated.h"


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class PREPPER_API UUnrealAIAmmoBoxComponent : public UAmmoBoxComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UUnrealAIAmmoBoxComponent();
	virtual int UseAmmo(EWeaponType WeaponType, int32 MaxUse) override;
};
