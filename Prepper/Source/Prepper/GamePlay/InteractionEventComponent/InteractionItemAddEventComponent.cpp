// Fill out your copyright notice in the Description page of Project Settings.


#include "InteractionItemAddEventComponent.h"

#include "Kismet/GameplayStatics.h"
#include "Prepper/GamePlay/CharacterController/CharacterController.h"
#include "Prepper/GamePlay/Item/Inventory/InventoryComponent.h"
#include "Sound/SoundCue.h"
#include "Prepper/___Legacy/Character/PlayerCharacter.h"


// Sets default values for this component's properties
UInteractionItemAddEventComponent::UInteractionItemAddEventComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	// ...
}

void UInteractionItemAddEventComponent::Interaction(APlayerCharacter* Target)
{
	if (!Target->GetInventory()->TryAddItem(ItemCode, 1))
	{
		return;
	}
	DestroyOwner();
}

void UInteractionItemAddEventComponent::Interaction(ICharacterController* Target)
{
	if (!Target->GetInventory()->TryAddItem(ItemCode, 1))
	{
		return;
	}
	DestroyOwner();
}

void UInteractionItemAddEventComponent::DestroyOwner()
{
	GetOwner()->Destroy(true);
}

void UInteractionItemAddEventComponent::DestroyComponent(bool bPromoteChildren)
{
	Super::DestroyComponent(bPromoteChildren);
	if (PickupSound == nullptr) return;
	
	UGameplayStatics::PlaySoundAtLocation(
		this,
		PickupSound,
		GetOwner()->GetActorLocation()
	);
}
