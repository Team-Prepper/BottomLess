// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/IUserObjectListEntry.h"
#include "Blueprint/UserWidget.h"
#include "ItemCombinationSelectUnitUI.generated.h"

class UItemCombinationSelectUI;
class UImage;
class UButton;
/**
 * 
 */
UCLASS()
class PREPPER_API UItemCombinationSelectUnitUI : public UUserWidget, public IUserObjectListEntry {
	GENERATED_BODY()
private:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> SelectButton;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> ItemIcon;
	
	UPROPERTY()
	TObjectPtr<UItemCombinationSelectUI> Parent;
	FString ItemCode;
	
public:
	virtual void NativeOnInitialized() override;
	virtual void NativeOnListItemObjectSet(UObject* ListItemObject) override;

	UFUNCTION()
	void Select();
};
