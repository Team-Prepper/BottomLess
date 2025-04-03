// Fill out your copyright notice in the Description page of Project Settings.


#include "InventoryUIUnit.h"

#include "InventoryUIUnitData.h"
#include "Components/Button.h"
#include "Components/CanvasPanel.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Prepper/__Legacy/Character/PlayerCharacter.h"
#include "Prepper/__Legacy/Item/ItemManager.h"

void UInventoryUIUnit::SetVisibility(ESlateVisibility InVisibility)
{
	Super::SetVisibility(InVisibility);
	if (InVisibility == ESlateVisibility::Visible) return;

	MenuPanel->SetVisibility(ESlateVisibility::Hidden);
}

void UInventoryUIUnit::NativeOnInitialized()
{
	Super::NativeOnInitialized();
	
	UseButton->OnClicked.AddDynamic(this, &UInventoryUIUnit::UseButtonAction);
	QuickSlotButton->OnClicked.AddDynamic(this, &UInventoryUIUnit::QuickSlotButtonAction);
	CancelButton->OnClicked.AddDynamic(this, &UInventoryUIUnit::CancelButtonAction);
	MenuButton->OnClicked.AddDynamic(this, &UInventoryUIUnit::MenuOpen);
}

void UInventoryUIUnit::NativeOnListItemObjectSet(UObject* ListItemObject)
{
	IUserObjectListEntry::NativeOnListItemObjectSet(ListItemObject);

	MenuPanel->SetVisibility(ESlateVisibility::Hidden);
	
	const UInventoryUIUnitData* Data = Cast<UInventoryUIUnitData>(ListItemObject);

	if (Data == nullptr) return;

	UTexture2D* Img;
	FText Name;
	
	TargetPlayer = Data->TargetPlayer;
	ItemCode = Data->ItemCode;

	if (Data->ItemCount >= 1)
	{
		ItemCount->SetText(FText::FromString(FString::Printf(TEXT("%d"), Data->ItemCount)));
	}
	else
	{
		ItemCount->SetText(FText::FromString(""));
	}
	
	if (!ItemManager::GetInstance()->GetItemData(Data->ItemCode, Img, Name) || Data->ItemCount < 1)
	{
		ItemIcon->SetBrushFromTexture(nullptr);
		ItemIcon->SetOpacity(0.f);
		return;
	}

	ItemIcon->SetOpacity(1.f);
	ItemIcon->SetBrushFromTexture(Img);
}

void UInventoryUIUnit::UseButtonAction()
{
	if (!TargetPlayer->GetInventory()->TryUseItem(ItemCode, 1)) return;
	
	FItem* Item = ItemManager::GetInstance()->GetItem(ItemCode);
	if (Item == nullptr) return;
	Item->Use(TargetPlayer);
	MenuPanel->SetVisibility(ESlateVisibility::Hidden);
}

void UInventoryUIUnit::CancelButtonAction()
{
	MenuPanel->SetVisibility(ESlateVisibility::Hidden);
}

void UInventoryUIUnit::QuickSlotButtonAction()
{
	IInventory* Inventory = TargetPlayer->GetInventory();
	
	Inventory->QuickSlotAdd(ItemCode, 0);
	MenuPanel->SetVisibility(ESlateVisibility::Hidden);
}

void UInventoryUIUnit::MenuOpen()
{
	MenuPanel->SetVisibility(ESlateVisibility::Visible);
}
