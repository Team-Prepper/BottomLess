// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InteractableItem.h"
#include "Prepper/__Legacy/Item/Inventory/MapInventory.h"
#include "InventoryInteractableItem.generated.h"

/**
 * 
 */
UCLASS()
class PREPPER_API AInventoryInteractableItem : public AInteractableItem
{
	GENERATED_BODY()
public:
	AInventoryInteractableItem();
	virtual void Interaction(APlayerCharacter* Target) override;

	virtual void ShowPickUpWidget(bool bShowWidget) override;

	void SetTargetInventory(UMapInventory* Inventory);

private:
	UPROPERTY()
	class UMapInventory* TargetInventory;
	
	UPROPERTY(VisibleAnywhere)
	class UBoxComponent* BoxCollision;

	UPROPERTY(VisibleAnywhere)
	class UWidgetComponent* WidgetComponent;
	
	UPROPERTY(EditAnywhere, Category = "UI")
	TSubclassOf<class UUserWidget> WidgetClass;

	UPROPERTY()
	class UUserWidget* WidgetInstance;

	void InitializeWidget();
};
