// Fill out your copyright notice in the Description page of Project Settings.


#include "ItemCombinationSelectUnitUI.h"

#include "ItemCombinationSelectUnitUIData.h"
#include "Components/Button.h"
#include "Components/Image.h"
#include "Prepper/__Legacy/Item/ItemManager.h"

void UItemCombinationSelectUnitUI::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	SelectButton->OnClicked.AddDynamic(this, &UItemCombinationSelectUnitUI::Select);
}

void UItemCombinationSelectUnitUI::NativeOnListItemObjectSet(UObject* ListItemObject)
{
	IUserObjectListEntry::NativeOnListItemObjectSet(ListItemObject);
	
	const  UItemCombinationSelectUnitUIData* Data =
		Cast<UItemCombinationSelectUnitUIData>(ListItemObject);

	if (Data == nullptr) return;

	Parent = Data->Parent;
	ItemCode = Data->ItemCode;
	
	UTexture2D* Img;
	FText Name;
	
	if (!ItemManager::GetInstance()->GetItemData(Data->ItemCode, Img, Name)) return;
	
	ItemIcon->SetBrushFromTexture(Img);
	
}

void UItemCombinationSelectUnitUI::Select()
{
	if (Parent == nullptr) return;
	Parent->SetGoalItem(ItemCode);
}
