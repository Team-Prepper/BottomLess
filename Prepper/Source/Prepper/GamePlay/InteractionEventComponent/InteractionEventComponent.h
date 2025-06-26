// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "InteractionEventComponent.generated.h"


class APlayerCharacter;
class ICharacterController;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class PREPPER_API UInteractionEventComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UInteractionEventComponent();
	
	virtual void Interaction(APlayerCharacter* Target);
	virtual void Interaction(ICharacterController* Target);
	
};