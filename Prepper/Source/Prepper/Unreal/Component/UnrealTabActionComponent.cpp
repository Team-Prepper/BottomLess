// Fill out your copyright notice in the Description page of Project Settings.


#include "UnrealTabActionComponent.h"


// Sets default values for this component's properties
UUnrealTabActionComponent::UUnrealTabActionComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	// ...
}

void UUnrealTabActionComponent::TabPressed()
{
	UE_LOG(LogTemp, Warning, TEXT("TabPressed"));
}

void UUnrealTabActionComponent::TabReleased()
{
	UE_LOG(LogTemp, Warning, TEXT("TabReleased"));
}

