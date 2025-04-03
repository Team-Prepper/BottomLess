// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InteractableObject.h"
#include "Prepper/__Legacy/Object/InteractableActor.h"
#include "ExitDoor.generated.h"

UCLASS()
class PREPPER_API AExitDoor : public AInteractableObject
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	FString TargetAchievement;
	
public:
	// Sets default values for this actor's properties
	AExitDoor();
	virtual void Interaction(APlayerCharacter* Target) override;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
};
