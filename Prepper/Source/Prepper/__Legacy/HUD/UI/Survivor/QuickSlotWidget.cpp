// Fill out your copyright notice in the Description page of Project Settings.


#include "QuickSlotWidget.h"

#include "Components/ListView.h"
#include "Prepper/__Legacy/HUD/UI/Inventory/InventoryUIUnitData.h"
#include "Prepper/__Legacy/Item/Inventory/Inventory.h"

void UQuickSlotUI::Update(IInventory* const& NewData)
{
	QuickSlotView->ClearListItems();
	TArray<FItemConvertData> QuickSlots = NewData->GetQuickSlotIter();
	
	UE_LOG(LogTemp, Warning, TEXT("QuickSlotCnt: %d"), QuickSlots.Num());
	
	for (int i = 0; i < QuickSlots.Num(); i++)
	{
		UInventoryUIUnitData* Data =
			NewObject<UInventoryUIUnitData>(GetWorld(), UInventoryUIUnitData::StaticClass());

		Data->ItemCode = QuickSlots[i].ItemCode;
		Data->ItemCount = QuickSlots[i].Count;
		Data->Idx = i;
		
		QuickSlotView->AddItem(Data);
	}
	
}
