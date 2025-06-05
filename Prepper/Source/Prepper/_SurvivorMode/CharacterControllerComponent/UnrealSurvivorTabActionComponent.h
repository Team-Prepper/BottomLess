// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Prepper/GamePlay/CharacterController/TabActionComponent.h"
#include "UnrealSurvivorTabActionComponent.generated.h"


class UInventoryUI;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class PREPPER_API UUnrealSurvivorTabActionComponent : public UTabActionComponent
{
	GENERATED_BODY()
	

	UPROPERTY(EditAnywhere, Category = "Player HUD")
	TSubclassOf<UInventoryUI> InventoryWidgetClass;
	UPROPERTY()
	TObjectPtr<UInventoryUI> InventoryWidget;
	
public:
	// Sets default values for this component's properties
	UUnrealSurvivorTabActionComponent();
	
	virtual void TabPressed() override;
	virtual void TabReleased() override;
};
