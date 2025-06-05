// Fill out your copyright notice in the Description page of Project Settings.


#include "InventoryQuickSlotUI.h"

#include "InventoryUIUnitData.h"
#include "Components/Button.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Prepper/GamePlay/Item/ItemManager.h"
#include "Prepper/GamePlay/Item/Inventory/InventoryComponent.h"
#include "Prepper/___Legacy/Character/PlayerCharacter.h"

void UInventoryQuickSlotUI::SetVisibility(ESlateVisibility InVisibility)
{
	Super::SetVisibility(InVisibility);
	if (InVisibility == ESlateVisibility::Visible) return;

	MenuPanel->SetVisibility(ESlateVisibility::Hidden);
}

void UInventoryQuickSlotUI::NativeOnInitialized()
{
	Super::NativeOnInitialized();
	
	QuickSlotRemoveButton->OnClicked.
			AddDynamic(this, &UInventoryQuickSlotUI::QuickSlotRemoveButtonAction);
	CancelButton->OnClicked.
			AddDynamic(this, &UInventoryQuickSlotUI::CancelButtonAction);
	MenuButton->OnClicked.
			AddDynamic(this, &UInventoryQuickSlotUI::MenuOpen);
}

void UInventoryQuickSlotUI::NativeOnListItemObjectSet(UObject* ListItemObject)
{
	IUserObjectListEntry::NativeOnListItemObjectSet(ListItemObject);

	MenuPanel->SetVisibility(ESlateVisibility::Hidden);
	
	const TObjectPtr<UInventoryUIUnitData> Data
			= Cast<UInventoryUIUnitData>(ListItemObject);

	if (Data == nullptr) return;

	UTexture2D* Img;
	FText Name;
	TargetPlayer = Data->TargetPlayer;
	ItemCode = Data->ItemCode;
	Idx = Data->Idx;

	if (Data->ItemCount >= 1)
	{
		ItemCount->SetText(FText::FromString(FString::Printf(TEXT("%d"), Data->ItemCount)));
	}
	else
	{
		DataExist = false;
		ItemCount->SetText(FText::FromString(""));
	}
	
	if (!ItemManager::GetInstance()->GetItemData(Data->ItemCode, Img, Name) || Data->ItemCount < 1)
	{
		ItemIcon->SetBrushFromTexture(nullptr);
		ItemIcon->SetOpacity(0.f);
		return;
	}

	DataExist = true;
	
	ItemIcon->SetOpacity(1.f);
	ItemIcon->SetBrushFromTexture(Img);
}

void UInventoryQuickSlotUI::QuickSlotRemoveButtonAction()
{
	UInventoryComponent* Inventory = TargetPlayer->GetInventory();

	Inventory->QuickSlotRemove(Idx);
	MenuPanel->SetVisibility(ESlateVisibility::Hidden);
}

void UInventoryQuickSlotUI::CancelButtonAction()
{
	MenuPanel->SetVisibility(ESlateVisibility::Hidden);
}

void UInventoryQuickSlotUI::MenuOpen()
{
	if (TargetPlayer == nullptr) return;
	if (!DataExist) return;
	MenuPanel->SetVisibility(ESlateVisibility::Visible);
}
