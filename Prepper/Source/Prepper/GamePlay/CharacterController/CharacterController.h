// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Prepper/GamePlay/Weapon/AmmoBox.h"
#include "UObject/Interface.h"
#include "CharacterController.generated.h"

class AItemBackpack;
class UInventoryComponent;
class ACar;
class UAmmoBoxComponent;
class UBCombatComponent;
class UStatusComponent;
class ICombat;
class IStatus;
class ABCharacter;
struct FInputActionValue;

// This class does not need to be modified.
UINTERFACE()
class UCharacterController : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class PREPPER_API ICharacterController
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	virtual TObjectPtr<APawn> GetPawn() PURE_VIRTUAL(ICharacterController::GetPawn, return nullptr; ) 
	virtual TObjectPtr<UStatusComponent> GetStatus() PURE_VIRTUAL(ICharacterController::GetStatus, return nullptr; )
	virtual TObjectPtr<UBCombatComponent> GetCombat() PURE_VIRTUAL(ICharacterController::GetCombat, return nullptr; )
	virtual TObjectPtr<UInventoryComponent> GetInventory() PURE_VIRTUAL(ICharacterController::GetInventory, return nullptr; )
	virtual IAmmoBox* GetAmmoBox() PURE_VIRTUAL(ICharacterController::GetAmmoBox, return nullptr; )

	virtual void EquipWeapon(TObjectPtr<AWeapon> Weapon) PURE_VIRTUAL();
	virtual void Boarding(TObjectPtr<ACar> Vehicle) PURE_VIRTUAL();
	virtual void GetOff() PURE_VIRTUAL();

	virtual void EquipBackpack(TObjectPtr<AItemBackpack> Backpack) PURE_VIRTUAL();
	
	virtual void Move(const FInputActionValue& Value) PURE_VIRTUAL();
	virtual void Look(const FInputActionValue& Value) PURE_VIRTUAL();
	virtual void CrouchToggle() PURE_VIRTUAL();
	virtual void JumpTrigger(bool IsTrigger) PURE_VIRTUAL();
	virtual void SprintTrigger(bool IsTrigger) PURE_VIRTUAL();
	virtual void EquipButtonPressed() PURE_VIRTUAL();
};
