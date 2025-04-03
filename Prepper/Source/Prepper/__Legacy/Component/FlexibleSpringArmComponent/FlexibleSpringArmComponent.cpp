// Fill out your copyright notice in the Description page of Project Settings.

#include "FlexibleSpringArmComponent.h"

UFlexibleSpringArmComponent::UFlexibleSpringArmComponent(const FObjectInitializer& ObjectInitializer /*= FObjectInitializer::Get()*/)
	: Super(ObjectInitializer)
{
	TargetLength = TargetArmLength;
	TargetLocation = GetRelativeLocation();
	
	Template.Add(FString("Default"), FFlexibleArmTemplate(350, FVector(0, 0, 100)));
}

void UFlexibleSpringArmComponent::BeginPlay()
{
	Super::BeginPlay();
	ChangeArmOffsetToTemplate(FString("Default"));
}

void UFlexibleSpringArmComponent::ChangeArmOffsetToTemplate(const FString& TemplateName)
{
	if (!Template.Contains(TemplateName)) return;

	const FFlexibleArmTemplate* Target = Template.Find(TemplateName);

	ChangeArmOffset(Target->Length, Target->Location);
}

void UFlexibleSpringArmComponent::ChangeArmOffset(const float Length, const FVector& Location)
{
	TargetLength = Length;
	TargetLocation = Location;

}

void UFlexibleSpringArmComponent::TickComponent(float DeltaTime, ELevelTick TickType,
	FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	
	SetRelativeLocation(FMath::VInterpTo(GetRelativeLocation(), TargetLocation, DeltaTime, InterpSpeed));
	TargetArmLength = FMath::FInterpTo(TargetArmLength, TargetLength, DeltaTime, InterpSpeed);
}