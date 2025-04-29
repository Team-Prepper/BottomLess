// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "Prepper/GamePlay/CharacterController/CharacterController.h"
#include "Prepper/__Legacy/Character/EnemyBaseCharacter.h"
#include "UnrealAIController.generated.h"

class UUnrealCombatComponent;
class UUnrealStatusComponent;
/**
 * 
 */
UCLASS()
class PREPPER_API AUnrealAIController : public AAIController
{
	GENERATED_BODY()

	TObjectPtr<ABCharacter> TargetCharacter;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	TObjectPtr<UUnrealStatusComponent> Status;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	TObjectPtr<UUnrealCombatComponent> Combat;
	
	UPROPERTY(EditAnywhere)
	float CombatRadius = 500.f;

	UPROPERTY(EditAnywhere)
	float AttackRadius = 150.f;
	
	UPROPERTY()
	TObjectPtr<APawn> PatrolTarget;
	
	EEnemyState EnemyState;

public:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;
	bool InTargetRange(TObjectPtr<AActor> Target, float Radius);
	virtual TObjectPtr<ABCharacter> GetTargetCharacter();
	
	UFUNCTION()
	void PawnSensingSeen(APawn* SeenPawn);
	UFUNCTION()
	void PawnSensingHeard(APawn* HeardPawn, const FVector& Location, float Volume);
};
