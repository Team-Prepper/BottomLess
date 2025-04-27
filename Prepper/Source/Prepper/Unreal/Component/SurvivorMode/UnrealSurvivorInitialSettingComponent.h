// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Prepper/Unreal/Component/UnrealInitialSettingComponent.h"
#include "UnrealSurvivorInitialSettingComponent.generated.h"


class UQuickSlotUI;
class UInventoryUI;
class UStatusWidget;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class PREPPER_API UUnrealSurvivorInitialSettingComponent : public UUnrealInitialSettingComponent
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, Category = "Player HUD")
	TSubclassOf<UStatusWidget> StatusWidgetClass;
	UPROPERTY()
	TObjectPtr<UStatusWidget> StatusWidget;

	UPROPERTY(EditAnywhere, Category = "Player HUD")
	TSubclassOf<UInventoryUI> InventoryWidgetClass;
	UPROPERTY()
	TObjectPtr<UInventoryUI> InventoryWidget;
	
	UPROPERTY(EditAnywhere, Category = "Player HUD")
	TSubclassOf<UQuickSlotUI> QuickSlotWidgetClass;
	UPROPERTY()
	TObjectPtr<UQuickSlotUI> QuickSlotWidget;

public:
	// Sets default values for this component's properties
	UUnrealSurvivorInitialSettingComponent();
	virtual void Initial(TObjectPtr<UnrealPlayerController> TargetCC) override;
};
