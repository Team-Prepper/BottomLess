// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Prepper/GamePlay/InteractionEventComponent/InteractionBoxOpenEventComponent.h"
#include "UnrealInteractionBoxOpenEventComponent.generated.h"


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class PREPPER_API UUnrealInteractionBoxOpenEventComponent : public UInteractionBoxOpenEventComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UUnrealInteractionBoxOpenEventComponent();
	virtual void BoxOpenAct() override;
	
	UFUNCTION(NetMulticast, Reliable)
	void MulticastBoxOpen();
};
