#pragma once

#include "CoreMinimal.h"
#include "Prepper/GamePlay/Object/InteractableActor.h"
#include "CraftingTable.generated.h"


class UItemCombinationUI;

UCLASS()
class PREPPER_API ACraftingTable : public AInteractableActor
{
	GENERATED_BODY()
	
	UPROPERTY(VisibleAnywhere, Category = "Mesh")
	TObjectPtr<UStaticMeshComponent> TableMesh;
	
public:
	ACraftingTable();
	
};