// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InteractableItem.h"
#include "ItemPickupAmmo.generated.h"

enum class EWeaponType : uint8;
/**
 * 
 */
UCLASS()
class PREPPER_API AItemPickupAmmo : public AInteractableItem
{
	GENERATED_BODY()
public:
	AItemPickupAmmo();

private:
	UPROPERTY(EditAnywhere)
	int32 AmmoAmount = 30;

	UPROPERTY(EditAnywhere)
	EWeaponType WeaponType;
	
};
