// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Prepper/GamePlay/CharacterController/StatusComponent.h"
#include "UnrealStatusComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PREPPER_API UUnrealStatusComponent : public UStatusComponent
{
	GENERATED_BODY()
	
	UPROPERTY(ReplicatedUsing = OnRep_Health, VisibleAnywhere, Category = "Player Stats")
	float NetworkCurrentHealth = 100.f;
	
	UFUNCTION()
	virtual void OnRep_Health();

public:	
	// Sets default values for this component's properties
	UUnrealStatusComponent();
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	virtual void TakeDamage(int Amount) override;
	virtual void AddHP(float Amount) override;	
	
};
