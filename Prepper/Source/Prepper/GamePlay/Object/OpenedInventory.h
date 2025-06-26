#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "OpenedInventory.generated.h"

class UInventoryComponent;
class UUnrealInventoryComponent;
class AInventoryInteractableItem;

UCLASS()
class PREPPER_API AOpenedInventory : public AActor
{
	GENERATED_BODY()
	
public:	
	AOpenedInventory();

	void SetTargetInventory(UInventoryComponent* Inventory);

	UFUNCTION()
	void CloseInventory();

protected:
	UFUNCTION()
	void InitInventory();

	UPROPERTY()
	uint32 RowSize = 3;

	UPROPERTY(EditAnywhere)
	float RowPivot = 50;

	UPROPERTY(EditAnywhere)
	float ColPivot = -180;

	UPROPERTY(EditAnywhere)
	float HeightPivot = 171;

	UPROPERTY()
	TObjectPtr<UInventoryComponent> TargetInventory;

	UPROPERTY()
	TObjectPtr<AInventoryInteractableItem> InventoryInteractableItem;
	
	UPROPERTY()
	TSubclassOf<AInventoryInteractableItem> InventoryInteractableItemClass;

	UPROPERTY()
	TArray<AActor*> SpawnedActors;
	
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USceneComponent> BaseActor;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UStaticMeshComponent> InventoryMesh;

public:
	FORCEINLINE TObjectPtr<UStaticMeshComponent> GetMesh() const { return InventoryMesh; }
	
};