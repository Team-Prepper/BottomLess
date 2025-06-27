// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Prepper/GamePlay/Object/InteractableActor.h"
#include "InteractableBox.generated.h"

class UInteractionBoxOpenEventComponent;

UCLASS()
class PREPPER_API AInteractableBox : public AInteractableActor
{
	GENERATED_BODY()

public:
	UPROPERTY(VisibleAnywhere, Category = "Mesh")
	TObjectPtr<UStaticMeshComponent> BoxMesh;
	
	AInteractableBox();
	
protected:
	virtual void BeginPlay() override;
	
};
