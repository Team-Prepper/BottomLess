// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Prepper/GamePlay/Interactable.h"
#include "InteractableActor.generated.h"

class UInteractionEventComponent;
class IPlayerAbility;
class UBoxComponent;
class UWidgetComponent;

UCLASS()
class PREPPER_API AInteractableActor : public AActor, public IInteractable
{
	GENERATED_BODY()
	
protected:
	UPROPERTY(EditAnywhere)
	FCollisionResponseContainer ColliderChannel;
	UPROPERTY(EditAnywhere)
	FCollisionResponseContainer TriggerChannel;
	
	UPROPERTY(VisibleAnywhere, Category = "Interaction Properties")
	TObjectPtr<UBoxComponent> AreaBox;
	UPROPERTY(VisibleAnywhere, Category = "Interaction Properties")
	TObjectPtr<UWidgetComponent> PickUpWidget;

	UPROPERTY(VisibleAnywhere, Category = "Interaction Properties")
	TObjectPtr<UInteractionEventComponent> InteractionEvent;
	
	UPROPERTY(EditAnywhere)
	int32 CustomDepthColor;

public:
	AInteractableActor();
	
	virtual void ShowPickUpWidget(bool bShowWidget) override;
	
	void ToggleTrigger(const bool bEnable);

	virtual void Interaction(APlayerCharacter* Target) override;
	virtual void Interaction(ICharacterController* Target) override;
	
protected:
	virtual void BeginPlay() override;
	
};