// Fill out your copyright notice in the Description page of Project Settings.


#include "InteractionAddAmmoEventComponent.h"

#include "Prepper/GamePlay/CharacterController/CharacterController.h"
#include "Prepper/___Legacy/Character/PlayerCharacter.h"
#include "Prepper/___Legacy/Character/Component/Combat/BaseCombatComponent.h"


// Sets default values for this component's properties
UInteractionAddAmmoEventComponent::UInteractionAddAmmoEventComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	// ...
}

void UInteractionAddAmmoEventComponent::Interaction(APlayerCharacter* Target)
{
	Super::Interaction(Target);

	if(Target)
	{
		AddAmmo(Target->GetCombatComponent());
	}
	GetOwner()->Destroy();
}

void UInteractionAddAmmoEventComponent::Interaction(ICharacterController* Target)
{
	Super::Interaction(Target);

	if(Target)
	{
		AddAmmo(Target->GetAmmoBox());
	}
	GetOwner()->Destroy();
}

void UInteractionAddAmmoEventComponent::AddAmmo(IAmmoBox* AmmoBox)
{
	if (AmmoBox == nullptr) return;
	AmmoBox->AddAmmo(WeaponType, AmmoAmount);
}
