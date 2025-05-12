// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "InventoryUIUnitData.generated.h"

class IPlayerAbility;
/**
 * 
 */
UCLASS()
class PREPPER_API UInventoryUIUnitData : public UObject
{
	GENERATED_BODY()
public:
	IPlayerAbility* TargetPlayer;
	FString ItemName;
	FString ItemCode;
	unsigned int ItemCount;
	int Idx;
};
