// Fill out your copyright notice in the Description page of Project Settings.


#include "ItemCombinationSelectUI.h"

#include "ItemCombinationSelectUnitUIData.h"
#include "ItemCombinationUI.h"
#include "Components/Button.h"
#include "Components/ListView.h"
#include "Prepper/__Legacy/HUD/UI/Inventory/InventoryUIUnitData.h"
#include "Prepper/__Legacy/Item/ItemManager.h"

void UItemCombinationSelectUI::NativeOnInitialized()
{
	Super::NativeOnInitialized();
	CloseButton->OnClicked.AddDynamic(this, &UItemCombinationSelectUI::Close);
	
	PossibleView->ClearListItems();

	TArray<FString> Possible = ItemManager::GetInstance()->GetPossibleCombination();
	
	for (int i = 0; i < Possible.Num(); i++)
	{
		UItemCombinationSelectUnitUIData* Data =
			NewObject<UItemCombinationSelectUnitUIData>(GetWorld(), UItemCombinationSelectUnitUIData::StaticClass());

		Data->ItemCode = Possible[i];
		Data->Parent = this;
		
		PossibleView->AddItem(Data);
		
	}
}

void UItemCombinationSelectUI::SetTarget(TObjectPtr<UItemCombinationUI> Target)
{
	TargetWidget = Target;
}

void UItemCombinationSelectUI::Close()
{
	RemoveFromParent();
}

void UItemCombinationSelectUI::SetGoalItem(const FString& GoalItemCode)
{
	TargetWidget->SetCombinationTarget(GoalItemCode);
	Close();
}