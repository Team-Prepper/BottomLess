// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Prepper/GamePlay/Car/PassengerComponent.h"
#include "UnrealPassengerComponent.generated.h"


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class PREPPER_API UUnrealPassengerComponent : public UPassengerComponent
{
	GENERATED_BODY()

	UPROPERTY(ReplicatedUsing=OnRep_Passenger)
	TArray<APawn*> ReplicatedPassengerList;

	UFUNCTION()
	void OnRep_Passenger();

public:
	// Sets default values for this component's properties
	UUnrealPassengerComponent();
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void AddPassenger(TObjectPtr<APawn> NewPassenger) override;
	virtual void RemovePassenger(TObjectPtr<APawn> TargetPassenger) override;

	UFUNCTION(Server, Reliable)
	void ServerRemovePassenger(APawn* TargetPassenger);
};
