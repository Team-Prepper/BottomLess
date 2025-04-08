// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PlayerAimingEffect.h"
#include "UObject/Object.h"
#include "ZoomInEffect.generated.h"

class APlayerCharacter;
class ABCharacter;
/**
 * 
 */
UCLASS(Blueprintable)
class PREPPER_API UZoomInEffect : public UPlayerAimingEffect
{
	GENERATED_BODY()
	UPROPERTY()
	TObjectPtr<APlayerCharacter> TargetPlayer;
	UPROPERTY()
	TObjectPtr<ABCharacter> TargetCharacter;
	
	UPROPERTY(EditAnywhere, Category=ZoomIn)
	float GoalFOV;
	UPROPERTY(EditAnywhere, Category=ZoomIn)
	float ZoomSpeed;
	
public:
	virtual void PlayerAimingStart(TObjectPtr<APlayerCharacter>) override;
	virtual void PlayerAimingEnd() override;
	virtual void CharacterAimingStart(const TObjectPtr<ABCharacter> Character) override;
	virtual void CharacterAimingEnd() override;
};
