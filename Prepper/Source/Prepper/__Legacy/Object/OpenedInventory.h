#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "OpenedInventory.generated.h"

class UInventoryComponent;
class UUnrealInventoryComponent;

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
	UInventoryComponent* TargetInventory;

	UPROPERTY()
	class AInventoryInteractableItem* InventoryInteractableItem;
	
	UPROPERTY()
	TSubclassOf<class AInventoryInteractableItem> InventoryInteractableItemClass;

	UPROPERTY()
	TArray<AActor*> SpawnedActors;
	
	UPROPERTY(VisibleAnywhere)
	class USceneComponent* BaseActor;

	UPROPERTY(VisibleAnywhere)
	class UStaticMeshComponent* InventoryMesh;

public:
	FORCEINLINE UStaticMeshComponent* GetMesh() const { return InventoryMesh; }
	
	
};
