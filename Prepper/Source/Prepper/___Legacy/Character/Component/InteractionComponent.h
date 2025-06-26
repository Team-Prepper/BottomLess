// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CharacterComponent.h"
#include "Components/ActorComponent.h"
#include "InteractionComponent.generated.h"


class IInteractable;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PREPPER_API UInteractionComponent : public UCharacterComponent
{
	GENERATED_BODY()

public:
	/* BASE */
	UInteractionComponent();
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
protected:
	/* SetInteractable */
	UFUNCTION()
	void SetItemInteractable(AActor* InteractableItem);
	UFUNCTION(Server, Reliable)
	void ServerSetItemInteractable(AActor* InteractableItem);

	UPROPERTY()
	ABaseCharacter* Character;
public:
	virtual void SetCharacter(ABaseCharacter* Target) override;
	virtual void TargetElim() override { };

	void TraceInteractionItem(FHitResult& TraceHitResult);

private:
	float TraceRange = 500.f;
public:
	UPROPERTY()
	TScriptInterface<IInteractable> CurInteractableItem;
		
};
