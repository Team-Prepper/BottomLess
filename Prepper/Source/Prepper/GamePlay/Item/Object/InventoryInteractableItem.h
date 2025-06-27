// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InteractableItem.h"
#include "InventoryInteractableItem.generated.h"

class UInventoryComponent;
class UBoxComponent;
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

	void SetTargetInventory(UInventoryComponent* Inventory);

private:
	UPROPERTY()
	UInventoryComponent* TargetInventory;
	
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UBoxComponent> BoxCollision;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UWidgetComponent> WidgetComponent;
	
	UPROPERTY(EditAnywhere, Category = "UI")
	TSubclassOf<UUserWidget> WidgetClass;

	UPROPERTY()
	TObjectPtr<UUserWidget> WidgetInstance;

	void InitializeWidget();
	
};
