// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "AmmoBox.generated.h"

enum class EWeaponType : uint8;
// This class does not need to be modified.
UINTERFACE()
class UAmmoBox : public UInterface
{
	GENERATED_BODY()
};

class PREPPER_API IAmmoBox
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	virtual void AddAmmo(EWeaponType WeaponType, int32 AmmoAmount) PURE_VIRTUAL();
	virtual int UseAmmo(EWeaponType WeaponType, int32 MaxUse) PURE_VIRTUAL( IAmmoBox::UseAmmo, return 0; )
};
