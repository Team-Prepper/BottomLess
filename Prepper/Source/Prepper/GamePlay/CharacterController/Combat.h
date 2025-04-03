// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "Combat.generated.h"

class AWeapon;
// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UCombat : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class PREPPER_API ICombat
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	virtual void EquipWeapon(AWeapon* Weapon) PURE_VIRTUAL();
	virtual void AttackTrigger(bool IsTrigger) PURE_VIRTUAL();
	virtual void AimTrigger(bool IsTrigger) PURE_VIRTUAL();
	virtual void Reload() PURE_VIRTUAL();
	
};
