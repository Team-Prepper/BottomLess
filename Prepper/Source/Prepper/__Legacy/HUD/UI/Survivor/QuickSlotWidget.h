// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Prepper/_Base/ObserverPattern/Observer.h"
#include "QuickSlotWidget.generated.h"

class UListView;
class IInventory;
class UTextBlock;
/**
 * 
 */
UCLASS()
class PREPPER_API UQuickSlotUI : public UUserWidget, public IObserver<IInventory*>
{
	GENERATED_BODY()
	
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UListView> QuickSlotView;
	
public:
	virtual void Update(IInventory* const& NewData) override;
};
