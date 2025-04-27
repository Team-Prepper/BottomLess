// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "UnrealInitialSettingComponent.generated.h"


class UnrealPlayerController;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class PREPPER_API UUnrealInitialSettingComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UUnrealInitialSettingComponent();
	virtual void Initial(TObjectPtr<UnrealPlayerController> TargetCC);
};
