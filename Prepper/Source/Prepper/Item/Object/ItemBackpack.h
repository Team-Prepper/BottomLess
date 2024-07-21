// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Prepper/Object/InteractableActor.h"
#include "ItemBackpack.generated.h"


UENUM(BlueprintType)
enum class EBackpackState : uint8
{
	EBS_Initial UMETA(DisplayName = "Initial State"),
	EBS_Equipped UMETA(DisplayName = "Equipped"),
	EBS_Dropped UMETA(DisplayName = "Dropped"),
	EBS_MAX UMETA(DisplayName = "Default MAX")
};

UCLASS()
class PREPPER_API AItemBackpack : public AInteractableActor
{
	GENERATED_BODY()
	
public:
	AItemBackpack();
	virtual void Interaction(APlayerCharacter* Target) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	void Dropped();

	UPROPERTY()
	USoundCue* EquipSound;

	/* Custom Depth 아이템 윤곽선 효과 */
	void EnableCustomDepth(bool bEnable);
	
protected:
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, Category = "Mesh")
	UStaticMeshComponent* BackpackMesh;

	UPROPERTY(Replicated)
	APlayerCharacter* PlayerOwnerCharacter;
	UPROPERTY()
	APrepperPlayerController* PlayerOwnerController;

	UPROPERTY(ReplicatedUsing = OnRep_BackpackState, VisibleAnywhere)
	EBackpackState BackpackState;

	UFUNCTION()
	void OnRep_BackpackState();

	void OnBackPackState();

	UPROPERTY(EditAnywhere)
	TSubclassOf<class AOpenedInventory> OpenedInventoryClass;

	UPROPERTY()
	UMapInventory* Inventory;
	
	UPROPERTY()
	AOpenedInventory* OpenedInventory;

	UPROPERTY(Replicated)
	bool IsOpened = false;

public:
	void ToggleInventory();
	void ShowInventory();
	void HideInventory();
	void SetBackpackState(EBackpackState NewBackpackState);
	FORCEINLINE UStaticMeshComponent* GetItemMesh() const { return BackpackMesh; }
	FORCEINLINE UMapInventory* GetInventory() const { return Inventory; }
};
