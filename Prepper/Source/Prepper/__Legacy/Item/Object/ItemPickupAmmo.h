// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InteractableItem.h"
#include "Prepper/__Legacy/Weapon/WeaponTypes.h"
#include "ItemPickupAmmo.generated.h"

/**
 * 
 */
UCLASS()
class PREPPER_API AItemPickupAmmo : public AInteractableItem
{
	GENERATED_BODY()
public:
	virtual void Interaction(APlayerCharacter *Target) override;
private:
	UPROPERTY(EditAnywhere)
	int32 AmmoAmount = 30;

	UPROPERTY(EditAnywhere)
	EWeaponType WeaponType;
	
};
