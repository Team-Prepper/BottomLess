// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ItemCombinationSelectUI.generated.h"

class UListView;
class UButton;
class UItemCombinationUI;
/**
 * 
 */
UCLASS()
class PREPPER_API UItemCombinationSelectUI : public UUserWidget
{
	GENERATED_BODY()
	TObjectPtr<UItemCombinationUI> TargetWidget;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> CloseButton;
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UListView> PossibleView;
public:
	virtual void NativeOnInitialized() override;
	void SetTarget(TObjectPtr<UItemCombinationUI> Target);
	UFUNCTION()
	void Close();
	void SetGoalItem(const FString& GoalItemCode);
};
