// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Prepper/__Legacy/Item/Inventory/Inventory.h"
#include "Prepper/_Base/ObserverPattern/Observer.h"
#include "InventoryUI.generated.h"

class UListView;
class UButton;
class APlayerCharacter;
/**
 * 
 */
UCLASS()
class PREPPER_API UInventoryUI : public UUserWidget, public IObserver<IInventory*>

{
	GENERATED_BODY()
private:
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UListView> QuickSlotView;
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UListView> InventoryView;
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UButton> CloseButton;

	UPROPERTY()
	TObjectPtr<APlayerCharacter> TargetPlayer;
public:
	virtual void NativeOnInitialized() override;
	virtual void SetVisibility(ESlateVisibility InVisibility) override;
	virtual void Update(IInventory* const& newData) override;
	void SetTargetPlayer(TObjectPtr<APlayerCharacter> Target);
	
	UFUNCTION()
	void Close();
	
};
