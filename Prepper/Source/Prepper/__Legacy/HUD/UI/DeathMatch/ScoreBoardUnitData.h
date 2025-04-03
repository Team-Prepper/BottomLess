// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "ScoreBoardUnitData.generated.h"

/**
 * 
 */
UCLASS()
class PREPPER_API UScoreBoardUnitData : public UObject
{
	GENERATED_BODY()
public:
	FString PlayerName;
	int Score;
	int Defeat;
};
