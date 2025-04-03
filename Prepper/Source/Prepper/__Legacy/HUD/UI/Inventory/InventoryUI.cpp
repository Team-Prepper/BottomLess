// Fill out your copyright notice in the Description page of Project Settings.


#include "InventoryUI.h"

#include "InventoryUIUnitData.h"
#include "Components/Button.h"
#include "Components/ListView.h"
#include "Kismet/GameplayStatics.h"
#include "Prepper/__Legacy/Character/PlayerCharacter.h"
#include "Prepper/__Legacy/Item/Inventory/Inventory.h"
#include "Prepper/__Legacy/Item/Inventory/MapInventory.h"

void UInventoryUI::NativeOnInitialized()
{
	Super::NativeOnInitialized();
	CloseButton->OnClicked.AddDynamic(this, &UInventoryUI::Close);
}

void UInventoryUI::SetVisibility(ESlateVisibility InVisibility)
{
	Super::SetVisibility(InVisibility);

	if (TargetPlayer == nullptr) return;
	APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	if (PC == nullptr) return;
	if (InVisibility == ESlateVisibility::Visible)
	{
		PC->SetInputMode(FInputModeGameAndUI());
		PC->SetShowMouseCursor(true);
		TargetPlayer->GetInventory()->Attach(this);
		SetFocus();
		return;
	}
	
	PC->SetInputMode(FInputModeGameOnly());
	PC->SetShowMouseCursor(false);
	TargetPlayer->GetInventory()->Detach(this);
	
}

void UInventoryUI::Update(IInventory* const& newData)
{
	InventoryView->ClearListItems();
	QuickSlotView->ClearListItems();

	TArray<FItemConvertData> Items = newData->GetIter();
	
	for (int i = 0; i < Items.Num(); i++)
	{
		UInventoryUIUnitData* Data =
			NewObject<UInventoryUIUnitData>(GetWorld(), UInventoryUIUnitData::StaticClass());

		Data->TargetPlayer = TargetPlayer;
		Data->ItemCode = Items[i].ItemCode;
		Data->ItemCount = Items[i].Count;
		Data->Idx = i;
		
		InventoryView->AddItem(Data);
	}
	
	TArray<FItemConvertData> QuickSlots = newData->GetQuickSlotIter();

	UE_LOG(LogTemp, Warning, TEXT("QuickSlotCnt: %d"), QuickSlots.Num());
	
	for (int i = 0; i < QuickSlots.Num(); i++)
	{
		UInventoryUIUnitData* Data =
			NewObject<UInventoryUIUnitData>(GetWorld(), UInventoryUIUnitData::StaticClass());

		Data->TargetPlayer = TargetPlayer;
		Data->ItemCode = QuickSlots[i].ItemCode;
		Data->ItemCount = QuickSlots[i].Count;
		Data->Idx = i;
		
		QuickSlotView->AddItem(Data);
	}
}

void UInventoryUI::SetTargetPlayer(TObjectPtr<APlayerCharacter> Target)
{
	TargetPlayer = Target;
}

void UInventoryUI::Close()
{
	SetVisibility(ESlateVisibility::Hidden);
}
