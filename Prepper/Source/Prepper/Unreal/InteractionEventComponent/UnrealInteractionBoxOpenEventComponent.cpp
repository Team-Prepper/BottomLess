// Fill out your copyright notice in the Description page of Project Settings.


#include "UnrealInteractionBoxOpenEventComponent.h"


// Sets default values for this component's properties
UUnrealInteractionBoxOpenEventComponent::UUnrealInteractionBoxOpenEventComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

}

void UUnrealInteractionBoxOpenEventComponent::BoxOpenAct()
{
	MulticastBoxOpen();
}

void UUnrealInteractionBoxOpenEventComponent::MulticastBoxOpen_Implementation()
{
	Super::BoxOpenAct();
}
