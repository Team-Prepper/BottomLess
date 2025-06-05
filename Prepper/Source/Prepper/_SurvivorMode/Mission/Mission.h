// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MissionChecker/MissionChecker.h"
#include "MissionReward/MissionReward.h"
#include "UObject/Object.h"
#include "Mission.generated.h"

/**
 * 
 */
UCLASS()
class PREPPER_API UMission : public UObject
{
	GENERATED_BODY()
private:
	IMissionChecker* Checker;
	IMissionReward* Reward;
	static TArray<FString> SplitString(const FString& Str);
	static IMissionChecker* GetMissionChecker(const FString& Code);
	static IMissionReward* GetMissionReward(const FString& Code);
public:
	UMission();
	UMission(const FString& CheckerCode, const FString& RewardCode);
};
