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
class PREPPER_API AUnrealAIController : public AAIController, public ICharacterController
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
	virtual TObjectPtr<ABCharacter> GetTargetCharacter() override;
	virtual IStatus* GetStatus() override;
	virtual ICombat* GetCombat() override;
	
	virtual void Move(const FInputActionValue& Value) override;
	virtual void Look(const FInputActionValue& Value) override;
	virtual void CrouchToggle() override;
	virtual void JumpTrigger(bool IsTrigger) override;
	virtual void SprintTrigger(bool IsTrigger) override;
	virtual void EquipButtonPressed() override;
	
	UFUNCTION()
	void PawnSensingSeen(APawn* SeenPawn);
	UFUNCTION()
	void PawnSensingHearn(APawn* HearnPawn, const FVector& Location, float Volume);
};
