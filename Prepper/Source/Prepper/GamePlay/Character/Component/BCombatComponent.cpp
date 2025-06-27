// Fill out your copyright notice in the Description page of Project Settings.


#include "BCombatComponent.h"

#include "Prepper/GamePlay/Weapon/Weapon.h"


// Sets default values for this component's properties
UBCombatComponent::UBCombatComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UBCombatComponent::SetCrosshairOverlay(TObjectPtr<UPlayerOverlay> Overlay)
{
	TargetOverlay = Overlay;
}

void UBCombatComponent::SetTargetCharacter(TObjectPtr<ABCharacter> Character)
{
	TargetCharacter = Character;
}

void UBCombatComponent::CharacterElim()
{
	if (EquippedWeapon != nullptr)
	{
		EquippedWeapon->OnDropped(TargetCharacter);
	}
	if (SecondaryWeapon != nullptr)
	{
		SecondaryWeapon->OnDropped(TargetCharacter);
	}
}

void UBCombatComponent::Attach(IObserver<UBCombatComponent>* Observer)
{
	Observers.Add(Observer);;
	Observer->Update(*this);
}

void UBCombatComponent::Detach(IObserver<UBCombatComponent>* Observer)
{
	Observers.Remove(Observer);
}

void UBCombatComponent::Notify()
{
	for (const auto Observer : Observers)
	{
		Observer->Update(*this);
	}
}