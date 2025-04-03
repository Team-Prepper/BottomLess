// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PlayerAimingEffect.h"
#include "SensitivityChangeEffect.generated.h"

/**
 * 
 */
UCLASS(Blueprintable)
class PREPPER_API USensitivityChangeEffect : public UPlayerAimingEffect
{
	GENERATED_BODY()
	UPROPERTY(EditAnywhere, meta = (ClampMin="0.0", ClampMax="1.0", UIMin="0.0", UIMax="1.0"))
	float Effect = 0.2f;
	TObjectPtr<APlayerCharacter> TargetPlayer;

public:
	virtual void PlayerAimingStart(TObjectPtr<APlayerCharacter>) override;
	virtual void PlayerAimingEnd() override;
	
};
