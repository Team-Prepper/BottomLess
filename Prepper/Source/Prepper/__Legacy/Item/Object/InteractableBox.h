// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Prepper/__Legacy/Object/InteractableActor.h"
#include "InteractableBox.generated.h"

UCLASS()
class PREPPER_API AInteractableBox : public AInteractableActor
{
	GENERATED_BODY()
public:
	AInteractableBox();
	virtual void Interaction(APlayerCharacter* Target) override;

	UPROPERTY(VisibleAnywhere, Category = "Mesh")
	UStaticMeshComponent* BoxMesh;

	bool IsOpen = false;

	void BoxOpen();

	UFUNCTION(NetMulticast, Reliable)
	void MulticastBoxOpen();

	UFUNCTION(NetMulticast, Reliable)
	void MulticastDestroyBox();

	FTimerHandle DestructionTimerHandle;
protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere)
	class UStaticMesh* OpenBoxMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawning")
	TArray<TSubclassOf<AActor>> SpawnedActorClasses;
	
};
