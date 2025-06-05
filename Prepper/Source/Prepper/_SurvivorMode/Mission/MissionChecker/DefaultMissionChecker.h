// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MissionChecker.h"

/**
#include "UObject/Object.h"
 * 
 */
class PREPPER_API UDefaultMissionChecker : public IMissionChecker
{
public:
	UDefaultMissionChecker(const TArray<FString>& Code);
	
	virtual void SetTargetMode(ASurvivorGameMode* Target) override;
	
	virtual bool CheckClear() override;
	virtual float GetClearRate() override;
	
	virtual FString GetMissionName() override;
	virtual FString GetMissionContent() override;
};
