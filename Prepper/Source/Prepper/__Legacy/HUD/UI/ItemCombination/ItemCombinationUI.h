// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Prepper/__Legacy/Item/Inventory/Inventory.h"
#include "Prepper/_Base/ObserverPattern/Observer.h"
#include "ItemCombinationUI.generated.h"

class UTextBlock;
class APlayerCharacter;
class UItemCombinationSelectUI;
class UImage;
class UButton;
/**
 * 
 */
UCLASS()
class PREPPER_API UItemCombinationUI : public UUserWidget, public IObserver<IInventory*>
{
	GENERATED_BODY()
private:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> SelectButton;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> CombinationButton;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> CloseButton;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> CntUp;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> CntDown;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> ResultIcon;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> SourceIcon1;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> SourceIcon2;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> ResultCnt;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> SourceCnt1;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> SourceCnt2;
	
	UPROPERTY(EditAnywhere, Category="Select")
	TSubclassOf<UItemCombinationSelectUI> SelectWidgetClass;
	UPROPERTY()
 	TObjectPtr<UItemCombinationSelectUI> SelectWidget;

	UPROPERTY()
	TObjectPtr<APlayerCharacter> TargetPlayer;
	FString TargetItem;
	FString SourceItem1;
	FString SourceItem2;

	int Cnt;
	int Cnt1;
	int Cnt2;
	
public:
	virtual void NativeOnInitialized() override;
	virtual void SetVisibility(ESlateVisibility InVisibility) override;
	virtual void Update(IInventory* const& newData) override;

	void SetTargetPlayer(TObjectPtr<APlayerCharacter> Target);
	
	void SetCombinationTarget(const FString& CombinationTarget);
	UFUNCTION()
	void OpenSelectWidget();
	UFUNCTION()
	void Combination();
	UFUNCTION()
	void Close();
	UFUNCTION()
	void CntUpAction();
	UFUNCTION()
	void CntDownAction();

	void CntSet(IInventory* Inventory);
};
