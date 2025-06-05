// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Prepper/__Base/ObserverPattern/Observer.h"
#include "InventoryUI.generated.h"

class IPlayerAbility;
class UInventoryComponent;
class UListView;
class UButton;
class APlayerCharacter;
/**
 * 
 */
UCLASS()
class PREPPER_API UInventoryUI : public UUserWidget, public IObserver<UInventoryComponent*>

{
	GENERATED_BODY()
private:
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UListView> QuickSlotView;
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UListView> InventoryView;
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UButton> CloseButton;

	IPlayerAbility* TargetPlayer;
public:
	virtual void NativeOnInitialized() override;
	virtual void SetVisibility(ESlateVisibility InVisibility) override;
	virtual void Update(UInventoryComponent* const& NewData) override;
	void SetTargetPlayer(IPlayerAbility* Target);
	
	UFUNCTION()
	void Close();
	
};
