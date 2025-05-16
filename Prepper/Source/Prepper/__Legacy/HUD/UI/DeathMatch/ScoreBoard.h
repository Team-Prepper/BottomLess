// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Prepper/_Base/ObserverPattern/Observer.h"
#include "UObject/Object.h"
#include "ScoreBoard.generated.h"

class ADeathMatchPlayerState;
class UListView;
/**
 * 
 */
UCLASS()
class PREPPER_API UScoreBoard : public UUserWidget, public IObserver<TArray<TObjectPtr<ADeathMatchPlayerState>>>
{
	GENERATED_BODY()
private:
	UPROPERTY(meta=(BindWidget))
	UListView* Ranking;
public:
	virtual void SetVisibility(ESlateVisibility InVisibility) override;
    virtual void Update(const TArray<TObjectPtr<ADeathMatchPlayerState>>& newData) override;
	
};
