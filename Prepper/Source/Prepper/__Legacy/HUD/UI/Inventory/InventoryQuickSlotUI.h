// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/IUserObjectListEntry.h"
#include "Blueprint/UserWidget.h"
#include "InventoryQuickSlotUI.generated.h"

class APlayerCharacter;
class UImage;
class UTextBlock;
class UButton;
/**
 * 
 */
UCLASS()
class PREPPER_API UInventoryQuickSlotUI : public UUserWidget, public IUserObjectListEntry
{
	GENERATED_BODY()
private:
	bool DataExist = false;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UWidget> MenuPanel;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> MenuButton;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> QuickSlotRemoveButton;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> CancelButton;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> ItemName;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> ItemIcon;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> ItemCount;

	UPROPERTY()
	TObjectPtr<APlayerCharacter> TargetPlayer;
	FString ItemCode;
	int Idx;

public:
	virtual void SetVisibility(ESlateVisibility InVisibility) override;
	virtual void NativeOnInitialized() override;
	virtual void NativeOnListItemObjectSet(UObject* ListItemObject) override;

	UFUNCTION()
	void MenuOpen();
	UFUNCTION()
	void CancelButtonAction();
	UFUNCTION()
	void QuickSlotRemoveButtonAction();
	
};
