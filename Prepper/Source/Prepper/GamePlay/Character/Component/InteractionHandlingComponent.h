// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "InteractionHandlingComponent.generated.h"


class ABCharacter;
class ICharacterController;
class IInteractable;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class PREPPER_API UInteractionHandlingComponent : public UActorComponent
{
	GENERATED_BODY()
private:
	UPROPERTY()
	TScriptInterface<IInteractable> CurInteractableItem;
	UPROPERTY()
	TObjectPtr<ABCharacter> TargetCharacter;
	
	float TraceRange = 500.f;
	
public:
	// Sets default values for this component's properties
	UInteractionHandlingComponent();
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
							   FActorComponentTickFunction* ThisTickFunction) override;
	
	void SetTargetCharacter(TObjectPtr<ABCharacter> Character);
	void TraceInteractionItem(FHitResult& TraceHitResult);
	void SetItemInteractable(TScriptInterface<IInteractable> InteractableItem);

	void Interaction();
	
	TScriptInterface<IInteractable> GetInteractable() const { return CurInteractableItem; }
};
