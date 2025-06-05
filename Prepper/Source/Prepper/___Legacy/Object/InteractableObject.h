// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InteractableActor.h"
#include "InteractableObject.generated.h"

/**
 * 
 */
UCLASS()
class PREPPER_API AInteractableObject : public AInteractableActor
{
	GENERATED_BODY()
	
public:	
	AInteractableObject();

	virtual void Interaction(APlayerCharacter *Target) override;
	
	UFUNCTION(BlueprintImplementableEvent)
	void InteractAction();

protected:
	virtual void BeginPlay() override;

private:
	UPROPERTY(VisibleAnywhere, Category = "Item Properties")
	USkeletalMeshComponent* ItemMesh;


};
