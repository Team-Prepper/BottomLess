﻿// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MissionData.generated.h"

USTRUCT(BlueprintType)
struct PREPPER_API FMissionData : public FTableRowBase
{
	GENERATED_BODY()
	
public:
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ItemInfo")
	FString ItemCode;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ItemInfo")
	FString MissionCode;

	FString GetItem() const
	{
		return MissionCode;
	}
};
