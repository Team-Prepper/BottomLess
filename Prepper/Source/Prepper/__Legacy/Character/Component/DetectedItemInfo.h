#pragma once

#include "CoreMinimal.h"
#include "DetectedItemInfo.generated.h"

USTRUCT(BlueprintType)
struct FDetectedItemInfo
{
	GENERATED_BODY()

	UPROPERTY()
	FString ItemName;
	UPROPERTY()
	float Distance;
	UPROPERTY()
	float LastDetectedTime;

	// 기본 생성자 추가
	FDetectedItemInfo(FString InName = "NaN", float InDistance = 0.0f, float InTime = 0.0f)
		: ItemName(InName)
		, Distance(InDistance)
		, LastDetectedTime(InTime)
	{}
};