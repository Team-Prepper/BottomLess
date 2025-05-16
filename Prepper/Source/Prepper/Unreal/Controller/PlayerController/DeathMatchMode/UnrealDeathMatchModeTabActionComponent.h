// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Prepper/Unreal/Controller/PlayerController/UnrealTabActionComponent.h"
#include "UObject/Object.h"
#include "UnrealDeathMatchModeTabActionComponent.generated.h"

class UScoreBoard;
/**
 * 
 */
UCLASS()
class PREPPER_API UUnrealDeathMatchModeTabActionComponent : public UUnrealTabActionComponent
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category = "Player HUD")
	TSubclassOf<UScoreBoard> ScoreBoardClass;
	UPROPERTY()
	TObjectPtr<UScoreBoard> ScoreBoard;
	
public:
	// Sets default values for this component's properties
	UUnrealDeathMatchModeTabActionComponent();
	
	virtual void TabPressed() override;
	virtual void TabReleased() override;
};
