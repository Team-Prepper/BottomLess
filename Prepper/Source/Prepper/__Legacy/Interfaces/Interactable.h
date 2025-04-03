// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PlayerAbility.h"
#include "UObject/Interface.h"
#include "Interactable.generated.h"

class APlayerCharacter;

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UInteractable : public UInterface
{
	GENERATED_BODY()
};

class PREPPER_API IInteractable
{
	GENERATED_BODY()

public:
	virtual void Interaction(APlayerCharacter* Target) PURE_VIRTUAL();
	virtual void ShowPickUpWidget(bool bShowWidget) PURE_VIRTUAL();
};
