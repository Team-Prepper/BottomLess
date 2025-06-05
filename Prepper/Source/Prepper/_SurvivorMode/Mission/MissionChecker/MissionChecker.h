#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"

class ASurvivorGameMode;

class PREPPER_API IMissionChecker
{
public:
	virtual ~IMissionChecker() = default;
	virtual void SetTargetMode(ASurvivorGameMode* Target) = 0;
	
	virtual bool CheckClear() = 0;
	virtual float GetClearRate() = 0;
	
	virtual FString GetMissionName() = 0;
	virtual FString GetMissionContent() = 0;
};