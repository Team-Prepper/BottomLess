// Fill out your copyright notice in the Description page of Project Settings.


#include "StatusComponent.h"


// Sets default values for this component's properties
UStatusComponent::UStatusComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	// ...
}

void UStatusComponent::Attach(IObserver<UStatusComponent>* Observer)
{
	Observers.Add(Observer);;
	UE_LOG(LogTemp, Warning, TEXT("CurrentHealth: %d"), GetCurHealth());
	Observer->Update(*this);
}

void UStatusComponent::Detach(IObserver<UStatusComponent>* Observer)
{
	Observers.Remove(Observer);
}

void UStatusComponent::Notify()
{
}