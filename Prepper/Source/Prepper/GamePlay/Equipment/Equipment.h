// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Prepper/GamePlay/Object/InteractableActor.h"
#include "Equipment.generated.h"

UCLASS()
class PREPPER_API AEquipment : public AActor, public IInteractable
{
	GENERATED_BODY()
	TArray<TObjectPtr<UMeshComponent>> MeshComponents;

protected:
	UPROPERTY(EditAnywhere)
	FCollisionResponseContainer ColliderChannel;
	UPROPERTY(EditAnywhere)
	FCollisionResponseContainer TriggerChannel;
	
	UPROPERTY(VisibleAnywhere, Category = "Interaction Properties")
	TObjectPtr<UBoxComponent> InteractionArea;
	UPROPERTY(VisibleAnywhere, Category = "Interaction Properties")
	TObjectPtr<UWidgetComponent> InteractionWidget;
	
	UPROPERTY(EditAnywhere)
	int32 CustomDepthColor;

	virtual void BeginPlay() override;
	
public:
	virtual void PostInitializeComponents() override;
	virtual void ShowPickUpWidget(bool bShowWidget) override;
	
	virtual FString GetCode();
	void ToggleOutline(const bool bEnable);
	void ToggleTrigger(const bool bEnable);
	
	
};
