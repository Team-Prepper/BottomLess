// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InventoryUIUnitData.h"
#include "Blueprint/IUserObjectListEntry.h"
#include "Blueprint/UserWidget.h"
#include "InventoryUIUnit.generated.h"

class APlayerCharacter;
class UButton;
class UCanvasPanel;
/**
 * 
 */
class UTextBlock;
class UImage;

UCLASS()
class PREPPER_API UInventoryUIUnit : public UUserWidget, public IUserObjectListEntry
{
	GENERATED_BODY()
private:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UWidget> MenuPanel;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> MenuButton;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> UseButton;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> CancelButton;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> QuickSlotButton;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> ItemName;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> ItemIcon;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> ItemCount;

	UPROPERTY()
	TObjectPtr<APlayerCharacter> TargetPlayer;
	FString ItemCode;

public:
	virtual void SetVisibility(ESlateVisibility InVisibility) override;
	virtual void NativeOnInitialized() override;
	virtual void NativeOnListItemObjectSet(UObject* ListItemObject) override;

	UFUNCTION()
	void MenuOpen();
	UFUNCTION()
	void UseButtonAction();
	UFUNCTION()
	void CancelButtonAction();
	UFUNCTION()
	void QuickSlotButtonAction();
	
};
