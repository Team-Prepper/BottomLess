// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "FlexibleArmTemplate.h"
#include "GameFramework/SpringArmComponent.h"
#include "FlexibleSpringArmComponent.generated.h"

/**
 * 
 */
UCLASS(ClassGroup=Camera, meta=(BlueprintSpawnableComponent), hideCategories=(Mobility))
class PREPPER_API UFlexibleSpringArmComponent : public USpringArmComponent
{
	GENERATED_BODY()
private:
	UPROPERTY(EditAnywhere, Category = FlexibleSpringArm)
	float InterpSpeed = 5.f;
	UPROPERTY(EditAnywhere, Category = FlexibleSpringArm)
	TMap<FString, FFlexibleArmTemplate> Template;
	
	float TargetLength;
	FVector TargetLocation;
protected:
	virtual void BeginPlay() override;
public:
	UFlexibleSpringArmComponent(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
	virtual void ChangeArmOffsetToTemplate(const FString& TemplateName);
	virtual void ChangeArmOffset(const float Length, const FVector& Location);
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
};