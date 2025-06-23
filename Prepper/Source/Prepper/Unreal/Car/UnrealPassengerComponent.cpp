// Fill out your copyright notice in the Description page of Project Settings.


#include "UnrealPassengerComponent.h"

#include "Net/UnrealNetwork.h"

// Sets default values for this component's properties
UUnrealPassengerComponent::UUnrealPassengerComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;
	SetIsReplicated(true);

	// ...
}

void UUnrealPassengerComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(UUnrealPassengerComponent, ReplicatedPassengerList);
}

void UUnrealPassengerComponent::OnRep_Passenger()
{
	PassengerList.Empty();
	for (auto Passenger : ReplicatedPassengerList)
	{
		PassengerList.Add(Passenger);
		UE_LOG(LogTemp, Warning, TEXT("Passenger: %s"), *Passenger->GetName());
	}
}

void UUnrealPassengerComponent::AddPassenger(TObjectPtr<APawn> NewPassenger)
{
	ReplicatedPassengerList.Add(NewPassenger);
	PassengerList.Add(NewPassenger);
	
	if (IsDriver(NewPassenger))
	{
		NewPassenger->GetController()->Possess(GetOwner<APawn>());
	}
	
}

void UUnrealPassengerComponent::RemovePassenger(TObjectPtr<APawn> TargetPassenger)
{
	ServerRemovePassenger(TargetPassenger);
}

void UUnrealPassengerComponent::ServerRemovePassenger_Implementation(APawn* TargetPassenger)
{
	const bool WasDriver = IsDriver(TargetPassenger);
	
	ReplicatedPassengerList.Remove(TargetPassenger);
	PassengerList.Remove(TargetPassenger);
	
	if (!WasDriver) return;
	
	const TObjectPtr<APawn> Owner = GetOwner<APawn>();
	
	Owner->GetController()->Possess(TargetPassenger);

	if (PassengerList.Num() < 1) return;
	
	PassengerList[0]->GetController()->Possess(Owner);
	
}