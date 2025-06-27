// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InteractionEventComponent.h"
#include "InteractionCraftingEventComponent.generated.h"

class UInventoryComponent;
class UItemCombinationUI;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class PREPPER_API UInteractionCraftingEventComponent : public UInteractionEventComponent
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category="Widget")
	TSubclassOf<UItemCombinationUI> CombinationWidgetClass;
	UPROPERTY()
	TObjectPtr<UItemCombinationUI> CombinationWidget;
	
	UPROPERTY(EditAnywhere)
	float Distance = 5.f;
	
	UPROPERTY()
	TObjectPtr<AActor> TargetPlayer;
	
public:
	// Sets default values for this component's properties
	UInteractionCraftingEventComponent();
	virtual void Interaction(APlayerCharacter* Target) override;
	virtual void Interaction(ICharacterController* Target) override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

protected:
	virtual void InteractionAct(TObjectPtr<APawn> Target, const TObjectPtr<UInventoryComponent> TargetInventory);
};
