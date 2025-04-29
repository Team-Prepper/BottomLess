// Fill out your copyright notice in the Description page of Project Settings.


#include "UnrealSurvivorInitialSettingComponent.h"

#include "Blueprint/UserWidget.h"
#include "Prepper/__Legacy/HUD/UI/CharacterOverlay/StatusWidget.h"
#include "Prepper/__Legacy/HUD/UI/Inventory/InventoryUI.h"
#include "Prepper/__Legacy/HUD/UI/Survivor/QuickSlotWidget.h"


// Sets default values for this component's properties
UUnrealSurvivorInitialSettingComponent::UUnrealSurvivorInitialSettingComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}

void UUnrealSurvivorInitialSettingComponent::Initial(TObjectPtr<UnrealPlayerController> TargetCC)
{
	Super::Initial(TargetCC);
	
	if (StatusWidgetClass && StatusWidget == nullptr)
	{
		StatusWidget = CreateWidget<UStatusWidget>(GetOwner<APlayerController>(), StatusWidgetClass);
		StatusWidget->AddToViewport();
	}
	
	if (QuickSlotWidgetClass && QuickSlotWidget == nullptr)
	{
		QuickSlotWidget = CreateWidget<UQuickSlotUI>(GetOwner<APlayerController>(), QuickSlotWidgetClass);
		QuickSlotWidget->AddToViewport();
	}
	
	if (InventoryWidgetClass && InventoryWidget == nullptr)
	{
		InventoryWidget = CreateWidget<UInventoryUI>(GetOwner<APlayerController>(), InventoryWidgetClass);
		InventoryWidget->SetVisibility(ESlateVisibility::Hidden);
		InventoryWidget->AddToViewport();
	}
}
