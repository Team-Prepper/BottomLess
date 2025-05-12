// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ItemCombinationSelectUI.h"
#include "ItemCombinationSelectUnitUIData.generated.h"

class UButton;
/**
 * 
 */
UCLASS()
class PREPPER_API UItemCombinationSelectUnitUIData : public UObject {
	GENERATED_BODY()
public:
	UPROPERTY()
	TObjectPtr<UItemCombinationSelectUI> Parent;
	FString ItemCode;
};