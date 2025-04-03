// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "PlayerAbility.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UPlayerAbility : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class PREPPER_API IPlayerAbility
{
	GENERATED_BODY()

public:
	virtual void AddItem(const FString& ItemCode, int Count) PURE_VIRTUAL();
	virtual void UseQuickSlotItem(int Idx) PURE_VIRTUAL();
	virtual void EquipWeapon(class AWeaponActor* Weapon) PURE_VIRTUAL();
	virtual void EquipBackpack(class AItemBackpack* BackpackToEquip) PURE_VIRTUAL();

	virtual void Heal(float Amount) PURE_VIRTUAL();
	virtual void Eat(float Amount) PURE_VIRTUAL();
	virtual void Drink(float Amount) PURE_VIRTUAL();
};
