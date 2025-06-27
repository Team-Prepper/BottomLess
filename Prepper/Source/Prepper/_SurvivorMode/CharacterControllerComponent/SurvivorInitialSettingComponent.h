// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Prepper/GamePlay/CharacterController/InitialSettingComponent.h"
#include "Prepper/__Base/ObserverPattern/Observer.h"
#include "SurvivorInitialSettingComponent.generated.h"

class ABCharacter;
struct FWeaponConvertData;
class UAmmoBoxComponent;
class UInventoryComponent;
class UQuickSlotUI;
class UInventoryUI;
class UStatusWidget;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class PREPPER_API USurvivorInitialSettingComponent : public UInitialSettingComponent
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category = "Player HUD")
	TSubclassOf<UStatusWidget> StatusWidgetClass;
	UPROPERTY()
	TObjectPtr<UStatusWidget> StatusWidget;
	
	UPROPERTY(EditAnywhere, Category = "Player HUD")
	TSubclassOf<UQuickSlotUI> QuickSlotWidgetClass;
	UPROPERTY()
	TObjectPtr<UQuickSlotUI> QuickSlotWidget;
	
public:
	// Sets default values for this component's properties
	USurvivorInitialSettingComponent();

protected:
	virtual void Attach() override;
	virtual void Detach() override;
	
public:
	virtual void WidgetSetting(TObjectPtr<AUnrealPlayerController> Target) override;
	virtual void Initial(TObjectPtr<AUnrealPlayerController> Target) override;
	
	void LoadClientData();
	void LoadServerData();
	
	UFUNCTION(Server, Reliable)
	void ServerAddItem(UInventoryComponent* TargetInventory, const FString& ItemCode, int ItemCount);

	UFUNCTION(Server, Reliable)
	void ServerSetAmmo(UAmmoBoxComponent* AmmoBox, const TArray<FWeaponConvertData>& AmmoMap);
	UFUNCTION(Server, Reliable)
	void ServerEquipEquipment(ABCharacter* Target, const FString& EquipmentCode);
	
};
