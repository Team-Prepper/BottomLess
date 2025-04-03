// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "PlayerAimingEffect.generated.h"

class APlayerCharacter;

UCLASS(Abstract, BlueprintType)
class PREPPER_API UPlayerAimingEffect : public UObject
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	virtual void PlayerAimingStart(TObjectPtr<APlayerCharacter>) PURE_VIRTUAL();
	virtual void PlayerAimingEnd() PURE_VIRTUAL();
};
