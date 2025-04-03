// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/ListView.h"
#include "UObject/Object.h"
#include "ScoreBoardUnit.generated.h"

/**
 * 
 */
class UTextBlock;

UCLASS()
class PREPPER_API UScoreBoardUnit : public UUserWidget, public IUserObjectListEntry
{
	GENERATED_BODY()
private:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> PlayerName;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> Score;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> Defeat;
public:
	virtual void NativeOnInitialized() override;
	virtual void NativeOnListItemObjectSet(UObject* ListItemObject) override;
	
};
