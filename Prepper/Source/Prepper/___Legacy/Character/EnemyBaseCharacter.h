// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseCharacter.h"
#include "Prepper/___Legacy/Interfaces/IDetectable.h"
#include "EnemyBaseCharacter.generated.h"

class AWeapon;

UENUM(BlueprintType)
enum class EEnemyState : uint8
{
	EES_Patrolling UMETA(DisplayName = "Patrolling"),
	EES_Chasing UMETA(DisplayName = "Chasing"),
	EES_Attacking UMETA(DisplayName = "Attacking")
};

UCLASS()
class PREPPER_API AEnemyBaseCharacter : public ABaseCharacter, public IDetectable
{
	GENERATED_BODY()
public:
	AEnemyBaseCharacter();
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	
	void CheckPatrolTarget();
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	// 감지태그 설정
	virtual FString GetDetectableDescription() const override
	{
		return "Zombie";
	}
	virtual EDetectableType GetDetectableType() const override 
	{ 
		return EDetectableType::Enemy; 
	}
protected:

	UPROPERTY(VisibleAnywhere)
	class UGaugeBarComponent* HealthBarWidget;
	
	UPROPERTY(VisibleAnywhere)
	class UPawnSensingComponent* PawnSensing;

	UPROPERTY(EditAnywhere)
	float CombatRadius = 500.f;

	UPROPERTY(EditAnywhere)
	float AttackRadius = 150.f;

	virtual void ReceiveDamage(float Damage, AController* InstigatorController, AActor* DamageCauser) override;

	/** 
	* Navigation
	*/

	UPROPERTY()
	class AAIController* EnemyController;

	// Current patrol target
	UPROPERTY(EditInstanceOnly, Category = "AI Navigation")
	AActor* PatrolTarget;

	UPROPERTY(EditInstanceOnly, Category = "AI Navigation")
	TArray<AActor*> PatrolTargets;

	UPROPERTY(EditAnywhere)
	double PatrolRadius = 200.f;

	FTimerHandle PatrolTimer;
	void PatrolTimerFinished();

	UPROPERTY(EditAnywhere, Category = "AI Navigation")
	float WaitMin = 5.f;

	UPROPERTY(EditAnywhere, Category = "AI Navigation")
	float WaitMax = 10.f;

	EEnemyState EnemyState = EEnemyState::EES_Patrolling;

	bool InTargetRange(const TObjectPtr<AActor> Target, const float Radius) const;
	bool TargetAlive(const TObjectPtr<AActor> Target) const;
	void MoveToTarget(AActor* Target);
	void MoveToLocation(FVector &Location);
	AActor* ChoosePatrolTarget();

	UFUNCTION()
	void PawnSeen(APawn* SeenPawn);
	UFUNCTION()
	void PawnHearn(APawn *HearnPawn, const FVector &Location, float Volume);

	virtual void Elim() override;

	/*
	 * Attack 
	 */

	UPROPERTY()
	bool bCanAttack = true;

	UFUNCTION(BlueprintCallable, Category = "Combat")
	void SpawnWeaponActor();

	UPROPERTY(EditAnywhere, Category = "Combat")
	TSubclassOf<AWeapon> WeaponActorClass;
	
};