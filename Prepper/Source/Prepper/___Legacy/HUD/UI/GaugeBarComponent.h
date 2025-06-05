// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/WidgetComponent.h"
#include "GaugeBarComponent.generated.h"

/**
 * 
 */
UCLASS()
class PREPPER_API UGaugeBarComponent : public UWidgetComponent
{
	GENERATED_BODY()
public:
	void SetGaugePercent(float Percent);
private:
	UPROPERTY()
	class UGaugeBar* GaugeBarWidget;
};