// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Prepper/GamePlay/CharacterController/TabActionComponent.h"
#include "UObject/Object.h"
#include "DeathMatchModeTabActionComponent.generated.h"

class UScoreBoard;
/**
 * 
 */
UCLASS()
class PREPPER_API UDeathMatchModeTabActionComponent : public UTabActionComponent
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category = "Player HUD")
	TSubclassOf<UScoreBoard> ScoreBoardClass;
	UPROPERTY()
	TObjectPtr<UScoreBoard> ScoreBoard;
	
public:
	// Sets default values for this component's properties
	UDeathMatchModeTabActionComponent();
	
	virtual void TabPressed() override;
	virtual void TabReleased() override;
};
