// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "DeathMatchWidget.generated.h"

class UTextBlock;
/**
 * 
 */
UCLASS()
class PREPPER_API UDeathMatchWidget : public UUserWidget
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, meta = (BindWidget), Category=DeathMatch)
	TObjectPtr<UTextBlock> MatchTime;
	UPROPERTY(EditAnywhere, meta = (BindWidget), Category=DeathMatch)
	TObjectPtr<UTextBlock> ScoreValue;
	UPROPERTY(EditAnywhere, meta = (BindWidget), Category=DeathMatch)
	TObjectPtr<UTextBlock> DefeatsValue;
	
public:
	void SetMatchTime(int Minutes, int Seconds);
	void SetScore(float Score) const;
	void SetDefeat(int Defeat) const;
};
