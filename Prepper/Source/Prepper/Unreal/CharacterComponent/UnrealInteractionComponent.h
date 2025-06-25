// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "UnrealInteractionComponent.generated.h"


class ABCharacter;
class ICharacterController;
class IInteractable;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class PREPPER_API UUnrealInteractionComponent : public UActorComponent
{
	GENERATED_BODY()
private:
	UPROPERTY()
	TScriptInterface<IInteractable> CurInteractableItem;
	
	TObjectPtr<ABCharacter> TargetCharacter;
	float TraceRange = 500.f;
	
public:
	// Sets default values for this component's properties
	UUnrealInteractionComponent();
	void SetTargetCharacter(TObjectPtr<ABCharacter> Character);
	TScriptInterface<IInteractable> GetInteractable() const { return CurInteractableItem; }

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
	                           FActorComponentTickFunction* ThisTickFunction) override;
	void TraceInteractionItem(FHitResult& TraceHitResult);
	void SetItemInteractable(TScriptInterface<IInteractable> InteractableItem);

	void Interaction();
};
