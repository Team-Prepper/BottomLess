// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Camera/CameraComponent.h"
#include "Components/ActorComponent.h"
#include "CustomCameraComponent.generated.h"


UCLASS( HideCategories=(Mobility, Rendering, LOD), Blueprintable, ClassGroup=Camera, meta=(BlueprintSpawnableComponent) )
class PREPPER_API UCustomCameraComponent : public UCameraComponent
{
	GENERATED_BODY()
private:
	float GoalFOV;
	float InterpSpeed;

public:	
	float DefaultFOV;
	// Sets default values for this component's properties
	UCustomCameraComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	virtual void InterpFOV(float GoalFOVSet, float InterpSpeedSet) ;
	virtual float GetDefaultFieldOfView() { return FieldOfView; }
};
