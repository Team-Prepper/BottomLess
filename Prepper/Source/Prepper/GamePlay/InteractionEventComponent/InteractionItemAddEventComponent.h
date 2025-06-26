// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InteractionEventComponent.h"
#include "InteractionItemAddEventComponent.generated.h"


class APlayerCharacter;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class PREPPER_API UInteractionItemAddEventComponent : public UInteractionEventComponent
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category = "Item Properties")
	FString ItemCode;
	UPROPERTY(EditAnywhere)
	TObjectPtr<USoundCue> PickupSound;
	
public:
	// Sets default values for this component's properties
	UInteractionItemAddEventComponent();
	
	virtual void Interaction(APlayerCharacter* Target) override;
	virtual void Interaction(ICharacterController* Target) override;
protected:
	virtual void DestroyOwner();
	virtual void DestroyComponent(bool bPromoteChildren) override;
};