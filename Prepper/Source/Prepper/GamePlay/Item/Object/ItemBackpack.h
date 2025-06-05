// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Prepper/GamePlay/Equipment/Equipment.h"
#include "Prepper/___Legacy/Object/InteractableActor.h"
#include "ItemBackpack.generated.h"


UENUM(BlueprintType)
enum class EBackpackState : uint8
{
	EBS_Initial UMETA(DisplayName = "Initial State"),
	EBS_Equipped UMETA(DisplayName = "Equipped"),
	EBS_Dropped UMETA(DisplayName = "Dropped"),
	EBS_OpenInventory UMETA(DisplayName = "OpenInventory"),
	EBS_MAX UMETA(DisplayName = "Default MAX")
};

UCLASS()
class PREPPER_API AItemBackpack : public AEquipment
{
	GENERATED_BODY()
	
public:
	AItemBackpack();
	virtual void Interaction(APlayerCharacter* Target) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	void Equip();
	void Dropped();

	UPROPERTY()
	USoundCue* EquipSound;

protected:
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, Category = "Mesh")
	UStaticMeshComponent* BackpackMesh;

	UPROPERTY(Replicated)
	APlayerCharacter* PlayerOwnerCharacter;

	UPROPERTY(ReplicatedUsing = OnRep_BackpackState, VisibleAnywhere)
	EBackpackState BackpackState;

	UFUNCTION()
	void OnRep_BackpackState();

	void OnBackPackState();

	UPROPERTY(EditAnywhere)
	TSubclassOf<class AOpenedInventory> OpenedInventoryClass;

	UPROPERTY()
	class UInventoryComponent* Inventory;
	
	UPROPERTY()
	AOpenedInventory* OpenedInventory;

	UPROPERTY(Replicated)
	bool IsOpened = false;
	
private:
	UPROPERTY(EditAnywhere, Category = "Backpack Properties")
	FString BackpackCode;
	
	void BackpackPhysicsActive(bool active);

public:
	virtual FString GetCode() override { return BackpackCode; }
	
	void OpenInventory();
	void ToggleInventory();
	void ShowInventory();
	void HideInventory();
	void SetBackpackState(EBackpackState NewBackpackState);
	FORCEINLINE UStaticMeshComponent* GetItemMesh() const { return BackpackMesh; }
	FORCEINLINE UInventoryComponent* GetInventory() const { return Inventory; }
};
