#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Prepper/___Legacy/Interfaces/IDetectable.h"
#include "Prepper/___Legacy/Object/InteractableActor.h"
#include "InteractableItem.generated.h"

UCLASS()
class PREPPER_API AInteractableItem : public AInteractableActor, public IDetectable
{
	GENERATED_BODY()
	
public:	
	AInteractableItem();
	virtual void Tick(float DeltaTime) override;
	virtual void Interaction(APlayerCharacter* Target) override;
	virtual void Destroyed() override;

	virtual FString GetDetectableDescription() const override
	{
		return ItemCode;
	}
	virtual EDetectableType GetDetectableType() const override 
	{ 
		return EDetectableType::Item; 
	}
protected:
	virtual void BeginPlay() override;
	
	UPROPERTY(VisibleAnywhere, Category = "Item Properties")
	UStaticMeshComponent* ItemMesh;
	
	UPROPERTY(EditAnywhere)
	float BaseTurnRate = 45.f;
	
	UPROPERTY(EditAnywhere, Category = "Item Properties")
	FString ItemCode;

	UPROPERTY(EditAnywhere)
	class USoundCue* PickupSound;
	
	void DestroyInteractionItem();
	UFUNCTION(NetMulticast, Reliable)
	void MulticastDestroyInteractionItem();
};