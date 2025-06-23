// Fill out your copyright notice in the Description page of Project Settings.


#include "PassengerComponent.h"


// Sets default values for this component's properties
UPassengerComponent::UPassengerComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

bool UPassengerComponent::IsDriver(const APawn* Compare)
{
	if (PassengerList.Num() < 1) return false;

	return PassengerList[0] == Compare;
}

bool UPassengerComponent::IsFull() const
{
	return Capacity <= PassengerList.Num();
}

void UPassengerComponent::AddPassenger(TObjectPtr<APawn> NewPassenger)
{
	PassengerList.Add(NewPassenger);
}

void UPassengerComponent::RemovePassenger(const TObjectPtr<APawn> TargetPassenger)
{
	if (PassengerList.Contains(TargetPassenger))
	{
		PassengerList.Remove(TargetPassenger);
	}
	//TargetPassenger->GetController()->Possess(nullptr);
}