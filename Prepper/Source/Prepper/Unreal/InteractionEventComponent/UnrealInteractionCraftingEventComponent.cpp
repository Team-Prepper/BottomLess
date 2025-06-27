// Fill out your copyright notice in the Description page of Project Settings.


#include "UnrealInteractionCraftingEventComponent.h"


// Sets default values for this component's properties
UUnrealInteractionCraftingEventComponent::UUnrealInteractionCraftingEventComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	SetIsReplicated(true);
	// ...
}

void UUnrealInteractionCraftingEventComponent::InteractionAct(const TObjectPtr<APawn> Target,
                                                              const TObjectPtr<UInventoryComponent> TargetInventory)
{
	MulticastInteractionAct(Target, TargetInventory);
}

void UUnrealInteractionCraftingEventComponent::MulticastInteractionAct_Implementation(APawn* Target,
	UInventoryComponent* TargetInventory)
{
	Super::InteractionAct(Target, TargetInventory);
}
