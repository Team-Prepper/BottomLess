#pragma once

#include "CoreMinimal.h"
#include "Prepper/___Legacy/Object/InteractableActor.h"
#include "CraftingTable.generated.h"


class UItemCombinationUI;

UCLASS()
class PREPPER_API ACraftingTable : public AInteractableActor
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category="Widget")
	TSubclassOf<UItemCombinationUI> CombinationWidgetClass;
	UPROPERTY()
	TObjectPtr<UItemCombinationUI> CombinationWidget;

	UPROPERTY()
	TObjectPtr<APlayerCharacter> TargetPlayer;

	UPROPERTY(EditAnywhere)
	float Distance = 5.f;
	
public:
	ACraftingTable();
	virtual void Interaction(APlayerCharacter* Target) override;
	virtual void Tick(float DeltaSeconds) override;

	UPROPERTY(VisibleAnywhere, Category = "Mesh")
	UStaticMeshComponent* TableMesh;

	UFUNCTION(NetMulticast, Reliable)
	void MulticastInteraction(APlayerCharacter* Target);
	
};