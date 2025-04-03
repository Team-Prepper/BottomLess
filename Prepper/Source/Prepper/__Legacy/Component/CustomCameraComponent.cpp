#include "CustomCameraComponent.h"

UCustomCameraComponent::UCustomCameraComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

}

void UCustomCameraComponent::BeginPlay()
{
	Super::BeginPlay();

	DefaultFOV = FieldOfView;
	GoalFOV = FieldOfView;
	
}

void UCustomCameraComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	
	SetFieldOfView(FMath::FInterpTo(FieldOfView, GoalFOV, DeltaTime, InterpSpeed));
}

void UCustomCameraComponent::InterpFOV(float GoalFOVSet, float InterpSpeedSet)
{
	GoalFOV = GoalFOVSet;
	InterpSpeed = InterpSpeedSet;
}

