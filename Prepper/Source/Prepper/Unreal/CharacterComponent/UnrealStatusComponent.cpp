// Fill out your copyright notice in the Description page of Project Settings.


#include "UnrealStatusComponent.h"

#include "AnimNodes/AnimNode_RandomPlayer.h"
#include "Net/UnrealNetwork.h"

void UUnrealStatusComponent::OnRep_Health()
{
	CurrentHealth = NetworkCurrentHealth;
	Notify();
}

void UUnrealStatusComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(UUnrealStatusComponent, NetworkCurrentHealth);
}

// Sets default values for this component's properties
UUnrealStatusComponent::UUnrealStatusComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;
	SetIsReplicated(true);

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

void UUnrealStatusComponent::TakeDamage(int Amount)
{
	Super::TakeDamage(Amount);
	NetworkCurrentHealth = CurrentHealth;

}

void UUnrealStatusComponent::AddHP(float Amount)
{
	Super::AddHP(Amount);
	NetworkCurrentHealth = CurrentHealth;
	
}

