// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Prepper/__Legacy/Object/InteractableActor.h"
#include "Equipment.generated.h"

UCLASS()
class PREPPER_API AEquipment : public AInteractableActor
{
	GENERATED_BODY()

public:
	virtual FString GetCode();
};
