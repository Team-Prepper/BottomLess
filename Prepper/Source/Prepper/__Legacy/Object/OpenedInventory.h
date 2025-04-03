#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "OpenedInventory.generated.h"

class UMapInventory;
UCLASS()
class PREPPER_API AOpenedInventory : public AActor
{
	GENERATED_BODY()
	
public:	
	AOpenedInventory();

	void SetTargetInventory(UMapInventory* Inventory);

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
	UMapInventory* TargetInventory;

	UPROPERTY()
	class AInventoryInteractableItem* InventoryInteractableItem;
	
	UPROPERTY()
	TSubclassOf<class AInventoryInteractableItem> InventoryInteractableItemClass;

	TArray<AActor*> SpawnedActors;
	
	UPROPERTY(VisibleAnywhere)
	class USceneComponent* BaseActor;

	UPROPERTY(VisibleAnywhere)
	class UStaticMeshComponent* InventoryMesh;

public:
	FORCEINLINE UStaticMeshComponent* GetMesh() const { return InventoryMesh; }
	
	
};
