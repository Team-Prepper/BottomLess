// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InteractionEventComponent.h"
#include "InteractionExitEventComponent.generated.h"


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class PREPPER_API UInteractionExitEventComponent : public UInteractionEventComponent
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere)
	FString TargetAchievement;

public:
	// Sets default values for this component's properties
	UInteractionExitEventComponent();
	virtual void Interaction(ICharacterController* Target) override;
};
