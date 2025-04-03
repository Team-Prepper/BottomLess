// Fill out your copyright notice in the Description page of Project Settings.


#include "UnrealStatusComponent.h"
#include "Net/UnrealNetwork.h"

void UUnrealStatusComponent::OnRep_Health()
{
}

void UUnrealStatusComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(UUnrealStatusComponent, CurrentHealth);
}

// Sets default values for this component's properties
UUnrealStatusComponent::UUnrealStatusComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

}

// Called when the game starts
void UUnrealStatusComponent::BeginPlay()
{
	Super::BeginPlay();

}


// Called every frame
void UUnrealStatusComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

}

int UUnrealStatusComponent::GetCurHealth()
{
	return CurrentHealth;
}

int UUnrealStatusComponent::GetMaxHealth()
{
	return MaxHealth;
}

void UUnrealStatusComponent::TakeDamage(int Amount)
{
	CurrentHealth -= Amount;

	if (CurrentHealth < 0) CurrentHealth = 0;
	if(CurrentHealth != 0.f) return;

	
	
}

