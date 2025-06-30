// Fill out your copyright notice in the Description page of Project Settings.


#include "UnrealHealthPointComponent.h"

#include "Net/UnrealNetwork.h"


void UUnrealHealthPointComponent::OnRep_Health()
{
	CurrentHealth = NetworkCurrentHealth;
	Notify();
}

// Sets default values for this component's properties
UUnrealHealthPointComponent::UUnrealHealthPointComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	SetIsReplicated(true);
	// ...
}

void UUnrealHealthPointComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(UUnrealHealthPointComponent, NetworkCurrentHealth);
}

void UUnrealHealthPointComponent::OnDeath()
{
	if (!GetOwner()->HasAuthority()) return;
	MulticastOnDeath();
}

void UUnrealHealthPointComponent::TakeDamage(int Amount)
{
	Super::TakeDamage(Amount);
	NetworkCurrentHealth = CurrentHealth;
}

void UUnrealHealthPointComponent::AddHP(float Amount)
{
	Super::AddHP(Amount);
	NetworkCurrentHealth = CurrentHealth;
}

void UUnrealHealthPointComponent::MulticastOnDeath_Implementation()
{
	Super::OnDeath();
}
