// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InteractionEventComponent.h"
#include "InteractionBoxOpenEventComponent.generated.h"


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class PREPPER_API UInteractionBoxOpenEventComponent : public UInteractionEventComponent
{
	GENERATED_BODY()
	
protected:
	UPROPERTY(EditAnywhere)
	TObjectPtr<UStaticMesh> OpenBoxMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawning")
	TArray<TSubclassOf<AActor>> SpawnedActorClasses;

	UPROPERTY()
	TObjectPtr<UStaticMeshComponent> BoxMesh;
	
	FTimerHandle DestructionTimerHandle;
	bool IsOpen = false;
	
public:
	// Sets default values for this component's properties
	UInteractionBoxOpenEventComponent();

	void SetBoxMesh(TObjectPtr<UStaticMeshComponent> TargetBoxMesh);
	
	virtual void Interaction(APlayerCharacter* Target) override;
	virtual void Interaction(ICharacterController* Target) override;
	void DestroyBox() const;
	
protected:
	virtual void BeginPlay() override;
	void BoxOpen();
	virtual void BoxOpenAct();
};
