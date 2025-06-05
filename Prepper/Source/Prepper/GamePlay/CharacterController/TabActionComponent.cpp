// Fill out your copyright notice in the Description page of Project Settings.


#include "TabActionComponent.h"


// Sets default values for this component's properties
UTabActionComponent::UTabActionComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	// ...
}

void UTabActionComponent::TabPressed()
{
	UE_LOG(LogTemp, Warning, TEXT("TabPressed"));
}

void UTabActionComponent::TabReleased()
{
	UE_LOG(LogTemp, Warning, TEXT("TabReleased"));
}

