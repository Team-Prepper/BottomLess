// Fill out your copyright notice in the Description page of Project Settings.


#include "QuickSlotWidget.h"

#include "Components/ListView.h"
#include "Prepper/GamePlay/Item/Inventory/InventoryComponent.h"
#include "Prepper/GamePlay/Item/Inventory/UI/InventoryUIUnitData.h"

void UQuickSlotUI::Update(UInventoryComponent* const& NewData)
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
