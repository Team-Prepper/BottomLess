// Fill out your copyright notice in the Description page of Project Settings.


#include "ItemCombinationUI.h"

#include "ItemCombinationSelectUI.h"
#include "Components/Button.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Kismet/GameplayStatics.h"
#include "Prepper/GamePlay/Item/ItemManager.h"
#include "Prepper/GamePlay/Item/Inventory/InventoryComponent.h"
#include "Prepper/___Legacy/Character/PlayerCharacter.h"

void UItemCombinationUI::NativeOnInitialized()
{
	Super::NativeOnInitialized();
	SelectButton->OnClicked.AddDynamic(this, &UItemCombinationUI::OpenSelectWidget);
	CombinationButton->OnClicked.AddDynamic(this, &UItemCombinationUI::Combination);
	CloseButton->OnClicked.AddDynamic(this, &UItemCombinationUI::Close);
	CntUp->OnClicked.AddDynamic(this, &UItemCombinationUI::CntUpAction);
	CntDown->OnClicked.AddDynamic(this, &UItemCombinationUI::CntDownAction);
}

void UItemCombinationUI::SetVisibility(ESlateVisibility InVisibility)
{
	Super::SetVisibility(InVisibility);

	if (TargetInventory == nullptr) return;

	const TObjectPtr<APlayerController> PC =
		UGameplayStatics::GetPlayerController(GetWorld(), 0);
	
	if (PC == nullptr) return;
	
	if (InVisibility == ESlateVisibility::Visible)
	{
		PC->SetInputMode(FInputModeGameAndUI());
		PC->SetShowMouseCursor(true);
		TargetInventory->Attach(this);
		SetFocus();
		
		return;
	}
	
	PC->SetInputMode(FInputModeGameOnly());
	PC->SetShowMouseCursor(false);
	TargetInventory->Detach(this);
	
}

void UItemCombinationUI::Update(UInventoryComponent* const& newData)
{
	CntSet(newData);
}

void UItemCombinationUI::SetTargetInventory(TObjectPtr<UInventoryComponent> Target)
{
	TargetInventory = Target;
	Cnt = 0;
}

void UItemCombinationUI::SetCombinationTarget(const FString& CombinationTarget)
{
	TargetItem = CombinationTarget;
	
	UTexture2D* Img;
	FText Name;
	if (!ItemManager::GetInstance()->GetItemData(TargetItem, Img, Name)) return;

	ResultIcon->SetBrushFromTexture(Img);

	ItemManager::GetInstance()->TryCombinationItem
	(TargetItem, SourceItem1, Cnt1,
		SourceItem2, Cnt2);
	
	if (!ItemManager::GetInstance()->GetItemData(SourceItem1, Img, Name)) return;

	SourceIcon1->SetBrushFromTexture(Img);
	
	if (!ItemManager::GetInstance()->GetItemData(SourceItem2, Img, Name)) return;
	
	SourceIcon2->SetBrushFromTexture(Img);

	Update(TargetInventory);
	
}
 
 void UItemCombinationUI::OpenSelectWidget()
 {
	SelectWidget = CreateWidget<UItemCombinationSelectUI>(this, SelectWidgetClass);
 
 	SelectWidget->AddToViewport();
 	SelectWidget->SetTarget(this);
 
 }

void UItemCombinationUI::Combination()
{
	if (Cnt < 1) return;
	
	bool b = TargetInventory->TryGetItemCount(SourceItem1) >= Cnt1 * Cnt
			&& TargetInventory->TryGetItemCount(SourceItem2) >= Cnt2 * Cnt;

	if (!b) return;

	TargetInventory->TryUseItem(SourceItem1, Cnt1 * Cnt);
	TargetInventory->TryUseItem(SourceItem2, Cnt2 * Cnt);
	
	TargetInventory->TryAddItem(TargetItem, Cnt);
	
	int AMax = TargetInventory->TryGetItemCount(SourceItem1) / Cnt1;
	int BMax = TargetInventory->TryGetItemCount(SourceItem2) / Cnt2;
	
	if (AMax < BMax) BMax = AMax;
	if (BMax < Cnt) Cnt = BMax;

	CntSet(TargetInventory);
}

void UItemCombinationUI::Close()
{
	SetVisibility(ESlateVisibility::Hidden);
	if (SelectWidget != nullptr)
	{
		SelectWidget->Close();
	}
}

void UItemCombinationUI::CntUpAction()
{
	bool b = TargetInventory->TryGetItemCount(SourceItem1) >= Cnt1 * (1 + Cnt)
			&& TargetInventory->TryGetItemCount(SourceItem2) >= Cnt2 * (1 + Cnt);

	if (!b) return;
	Cnt++;
	CntSet(TargetInventory);
}

void UItemCombinationUI::CntDownAction()
{
	if (Cnt < 1) return;
	Cnt--;
	CntSet(TargetInventory);
}

void UItemCombinationUI::CntSet(UInventoryComponent* Inventory)
{
	ResultCnt->SetText(FText::FromString(FString::Printf(TEXT("%d"), Cnt)));
	
	SourceCnt1->SetText(FText::FromString(FString::Printf(TEXT("%d / %d"),
		Cnt1 * Cnt, Inventory->TryGetItemCount(SourceItem1))));
	SourceCnt2->SetText(FText::FromString(FString::Printf(TEXT("%d / %d"),
		Cnt2 * Cnt, Inventory->TryGetItemCount(SourceItem2))));
}