// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PlayerAimingEffect.h"
#include "UObject/Object.h"
#include "SniperWidgetEffect.generated.h"

/**
 * 
 */
UCLASS(Blueprintable)
class PREPPER_API USniperWidgetEffect : public UPlayerAimingEffect
{
	GENERATED_BODY()
	UPROPERTY(EditAnywhere, Category="SniperWidget")
	TSubclassOf<UUserWidget> SniperWidgetClass;
	UPROPERTY()
	TObjectPtr<UUserWidget> SniperWidget;
	
public:
	virtual void PlayerAimingStart(TObjectPtr<APlayerCharacter>) override;
	virtual void PlayerAimingEnd() override;
};
