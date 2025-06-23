// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "PassengerComponent.generated.h"


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class PREPPER_API UPassengerComponent : public UActorComponent
{
	GENERATED_BODY()

protected:
	UPROPERTY()
	TArray<APawn*> PassengerList;

	UPROPERTY(EditAnywhere)
	int Capacity = 1;

public:
	// Sets default values for this component's properties
	UPassengerComponent();

	bool IsDriver(const APawn* Compare);
	bool IsFull() const;
	virtual void AddPassenger(TObjectPtr<APawn> NewPassenger);
	virtual void RemovePassenger(TObjectPtr<APawn> TargetPassenger);
};
