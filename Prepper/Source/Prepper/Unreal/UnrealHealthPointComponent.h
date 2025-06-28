// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Prepper/GamePlay/HealthPointComponent/HealthPointComponent.h"
#include "UnrealHealthPointComponent.generated.h"


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class PREPPER_API UUnrealHealthPointComponent : public UHealthPointComponent
{
	GENERATED_BODY()

	UPROPERTY(ReplicatedUsing = OnRep_Health, VisibleAnywhere, Category = "Player Stats")
	float NetworkCurrentHealth = 100.f;
	
	UFUNCTION()
	virtual void OnRep_Health();
	
public:
	// Sets default values for this component's properties
	UUnrealHealthPointComponent();
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	virtual void TakeDamage(int Amount) override;
	virtual void AddHP(float Amount) override;	
};
