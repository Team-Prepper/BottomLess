// Fill out your copyright notice in the Description page of Project Settings.


#include "UnrealSurvivorTabActionComponent.h"

#include "Blueprint/UserWidget.h"
#include "Prepper/__Legacy/HUD/UI/Inventory/InventoryUI.h"


// Sets default values for this component's properties
UUnrealSurvivorTabActionComponent::UUnrealSurvivorTabActionComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	// ...
}

void UUnrealSurvivorTabActionComponent::TabPressed()
{
	if (InventoryWidgetClass != nullptr && InventoryWidget == nullptr)
	{
		InventoryWidget = CreateWidget<UInventoryUI>(GetOwner<APlayerController>(), InventoryWidgetClass);
		InventoryWidget->AddToViewport();
	}
	if (InventoryWidget == nullptr) return;
	
	InventoryWidget->SetVisibility(ESlateVisibility::Visible);
}

void UUnrealSurvivorTabActionComponent::TabReleased()
{
	Super::TabReleased();
}
