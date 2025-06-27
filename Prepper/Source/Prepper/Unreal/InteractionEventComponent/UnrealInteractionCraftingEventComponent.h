// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Prepper/GamePlay/InteractionEventComponent/InteractionCraftingEventComponent.h"
#include "UnrealInteractionCraftingEventComponent.generated.h"


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class PREPPER_API UUnrealInteractionCraftingEventComponent : public UInteractionCraftingEventComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UUnrealInteractionCraftingEventComponent();
protected:
	virtual void InteractionAct(TObjectPtr<APawn> Target, const TObjectPtr<UInventoryComponent> TargetInventory) override;

	UFUNCTION(NetMulticast, Reliable)
	void MulticastInteractionAct(APawn* Target, UInventoryComponent* TargetInventory);
};
