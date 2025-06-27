// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Prepper/GamePlay/Character/Component/ElimDissolveComponent.h"
#include "UnrealElimDissolveComponent.generated.h"


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class PREPPER_API UUnrealElimDissolveComponent : public UElimDissolveComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UUnrealElimDissolveComponent();
	virtual void TargetElim() override;
	
	UFUNCTION(NetMulticast, Reliable)
	void MulticastTargetElim();
};