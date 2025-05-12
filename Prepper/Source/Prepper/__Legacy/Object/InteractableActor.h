// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Prepper/GamePlay/Interactable.h"
#include "InteractableActor.generated.h"

class IPlayerAbility;
class UBoxComponent;
class UWidgetComponent;

UCLASS()
class PREPPER_API AInteractableActor : public AActor, public IInteractable
{
	GENERATED_BODY()
private:
	TArray<TObjectPtr<UMeshComponent>> MeshComponents;

protected:
	UPROPERTY(EditAnywhere)
	FCollisionResponseContainer ColliderChannel;
	UPROPERTY(EditAnywhere)
	FCollisionResponseContainer TriggerChannel;
	
public:
	AInteractableActor();
	virtual void PostInitializeComponents() override;
	
	virtual void ShowPickUpWidget(bool bShowWidget) override;
	void ToggleOutline(const bool bEnable);
	void ToggleTrigger(const bool bEnable);
	
protected:
	UPROPERTY(VisibleAnywhere, Category = "Trigger")
	TObjectPtr<UBoxComponent> AreaBox;
	
	UPROPERTY(VisibleAnywhere, Category = "Pickup Properties")
	TObjectPtr<UWidgetComponent> PickUpWidget;
	
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere)
	int32 CustomDepthColor;
	
};