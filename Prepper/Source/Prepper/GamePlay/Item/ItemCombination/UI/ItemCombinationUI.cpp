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

	if (TargetPlayer == nullptr) return;

	const TObjectPtr<APlayerController> PC =
		UGameplayStatics::GetPlayerController(GetWorld(), 0);
	
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

void UItemCombinationUI::Update(UInventoryComponent* const& newData)
{
	CntSet(newData);
}

void UItemCombinationUI::SetTargetPlayer(TObjectPtr<APlayerCharacter> Target)
{
	TargetPlayer = Target;
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

	Update(TargetPlayer->GetInventory());
	
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
	
	bool b = TargetPlayer->GetInventory()->TryGetItemCount(SourceItem1) >= Cnt1 * Cnt
			&& TargetPlayer->GetInventory()->TryGetItemCount(SourceItem2) >= Cnt2 * Cnt;

	if (!b) return;

	TargetPlayer->GetInventory()->TryUseItem(SourceItem1, Cnt1 * Cnt);
	TargetPlayer->GetInventory()->TryUseItem(SourceItem2, Cnt2 * Cnt);
	
	TargetPlayer->GetInventory()->TryAddItem(TargetItem, Cnt);
	
	int AMax = TargetPlayer->GetInventory()->TryGetItemCount(SourceItem1) / Cnt1;
	int BMax = TargetPlayer->GetInventory()->TryGetItemCount(SourceItem2) / Cnt2;
	
	if (AMax < BMax) BMax = AMax;
	if (BMax < Cnt) Cnt = BMax;

	CntSet(TargetPlayer->GetInventory());
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
	bool b = TargetPlayer->GetInventory()->TryGetItemCount(SourceItem1) >= Cnt1 * (1 + Cnt)
			&& TargetPlayer->GetInventory()->TryGetItemCount(SourceItem2) >= Cnt2 * (1 + Cnt);

	if (!b) return;
	Cnt++;
	CntSet(TargetPlayer->GetInventory());
}

void UItemCombinationUI::CntDownAction()
{
	if (Cnt < 1) return;
	Cnt--;
	CntSet(TargetPlayer->GetInventory());
}

void UItemCombinationUI::CntSet(UInventoryComponent* Inventory)
{
	ResultCnt->SetText(FText::FromString(FString::Printf(TEXT("%d"), Cnt)));
	
	SourceCnt1->SetText(FText::FromString(FString::Printf(TEXT("%d / %d"),
		Cnt1 * Cnt, Inventory->TryGetItemCount(SourceItem1))));
	SourceCnt2->SetText(FText::FromString(FString::Printf(TEXT("%d / %d"),
		Cnt2 * Cnt, Inventory->TryGetItemCount(SourceItem2))));
}