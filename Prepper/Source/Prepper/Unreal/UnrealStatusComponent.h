// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Prepper/GamePlay/CharacterController/Status.h"
#include "UnrealStatusComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PREPPER_API UUnrealStatusComponent :
		public UActorComponent, public IStatus
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, Category = "Player Stats")
	float MaxHealth = 100.f;
	UPROPERTY(ReplicatedUsing = OnRep_Health, VisibleAnywhere, Category = "Player Stats")
	float CurrentHealth = 100.f;
	
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

	virtual int GetCurHealth() override;
	virtual int GetMaxHealth() override;
	virtual void TakeDamage(int Amount) override;
	
};
