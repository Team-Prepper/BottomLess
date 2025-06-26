#include "InventoryInteractableItem.h"

#include "Blueprint/UserWidget.h"
#include "Components/BoxComponent.h"
#include "Components/WidgetComponent.h"
#include "Prepper/GamePlay/Item/ItemManager.h"
#include "Prepper/GamePlay/Item/Inventory/InventoryComponent.h"
#include "Prepper/___Legacy/HUD/UI/InventoryInteractionUI.h"
#include "Prepper/___Legacy/Character/PlayerCharacter.h"

AInventoryInteractableItem::AInventoryInteractableItem()
{
	TargetInventory = nullptr;
	BoxCollision = nullptr;
	WidgetComponent = nullptr;
	WidgetInstance = nullptr;
	PrimaryActorTick.bCanEverTick = false;
}

void AInventoryInteractableItem::SetTargetInventory(UInventoryComponent* Inventory)
{
	TargetInventory = Inventory;
}

void AInventoryInteractableItem::Interaction(APlayerCharacter* Target)
{
	if (!TargetInventory->TryUseItem(ItemCode, 1)) return;
	
	ItemManager::GetInstance()->GetItem(ItemCode)->Use(Cast<IPlayerAbility>(Target));
	if (TargetInventory->TryGetItemCount(ItemCode) == 0)
	{
		//DestroyInteractionItem();
	}
}

void AInventoryInteractableItem::ShowPickUpWidget(bool bShowWidget)
{
	UE_LOG(LogTemp, Warning, TEXT("toggle Pickup widget"));
	PickUpWidget->SetVisibility(bShowWidget);
}

void AInventoryInteractableItem::InitializeWidget()
{
	if (!PickUpWidget || !PickUpWidget->GetUserWidgetObject()) return;
	if (PickUpWidget && PickUpWidget->GetUserWidgetObject()) return;
	
	UUserWidget* Widget = PickUpWidget->GetUserWidgetObject();
	if (!Widget) return;
	
	UInventoryInteractionUI* ItemInfoWidget = Cast<UInventoryInteractionUI>(Widget);
	if (ItemInfoWidget)
	{
		ItemInfoWidget->InitializeWidget(ItemCode);
	}
}
