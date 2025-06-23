// Fill out your copyright notice in the Description page of Project Settings.


#include "UnrealAIController.h"

#include "AmmoBox/UnrealAIAmmoBoxComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Navigation/PathFollowingComponent.h"
#include "Perception/PawnSensingComponent.h"
#include "Prepper/GamePlay/Character/BCharacter.h"
#include "Prepper/GamePlay/Character/Component/BCombatComponent.h"

AUnrealAIController::AUnrealAIController()
{
	PrimaryActorTick.bCanEverTick = true;
	EnemyState = EEnemyState::EES_Patrolling;
	
	AmmoBox = CreateDefaultSubobject<UUnrealAIAmmoBoxComponent>(TEXT("AmmoBoxComponent"));
}

void AUnrealAIController::BeginPlay()
{
	Super::BeginPlay();
	
}

void AUnrealAIController::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	
	if (GetTargetCharacter() == nullptr) return;

	if (PatrolTarget == nullptr)
	{
		return;
	}

	const float PatrolTargetSqrDiff = LocationSqrDiff(PatrolTarget->GetActorLocation());
	
	if (PatrolTargetSqrDiff < AttackRadius * AttackRadius)
	{
		EnemyState = EEnemyState::EES_Attacking;
		
		GetTargetCharacter()->GetCharacterMovement()->StopMovementImmediately();
		
		GetTargetCharacter()->GetCombat()->AttackTrigger(true);
		GetTargetCharacter()->GetCombat()->AttackTrigger(false);
		
		return;
	}

	MoveToActor(PatrolTarget, 1.5f);
	
	if (PatrolTargetSqrDiff <= CombatRadius * CombatRadius)
	{
		EnemyState = EEnemyState::EES_Chasing;
		GetTargetCharacter()->SprintTrigger(true);
		return;
	}

	EnemyState = EEnemyState::EES_Patrolling;
	GetTargetCharacter()->SprintTrigger(false);
	
}

void AUnrealAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
	
	if (!HasAuthority()) return;
	if (GetTargetCharacter() == nullptr) return;
	
	UE_LOG(LogTemp, Warning, TEXT("OnPossess: %s"), *GetTargetCharacter()->GetName());
	
	GetTargetCharacter()->SetAmmoBox(AmmoBox);
	
	GetTargetCharacter()->GetPawnSensing()->SetComponentTickEnabled(true);
	GetTargetCharacter()->GetPawnSensing()->SightRadius = 4000.f;
	GetTargetCharacter()->GetPawnSensing()->SetPeripheralVisionAngle(100.f);
	
	GetTargetCharacter()->GetPawnSensing()->OnSeePawn.AddDynamic(this, &AUnrealAIController::PawnSensingSeen);
	GetTargetCharacter()->GetPawnSensing()->OnHearNoise.AddDynamic(this, &AUnrealAIController::PawnSensingHeard);
}

float AUnrealAIController::LocationSqrDiff(const FVector& Location) const
{
	return (Location - GetPawn()->GetActorLocation()).SizeSquared();
}

TObjectPtr<ABCharacter> AUnrealAIController::GetTargetCharacter()
{
	if (TargetCharacter == nullptr)
	{
		TargetCharacter = GetPawn<ABCharacter>();
	}
	return TargetCharacter;
}

void AUnrealAIController::PawnSensingSeen(APawn* SeenPawn)
{
	if (EnemyState == EEnemyState::EES_Attacking) return;
	
	const TObjectPtr<ABCharacter> SeenCharacter = Cast<ABCharacter>(SeenPawn);
	
	if (SeenCharacter == nullptr) return;
	if (SeenCharacter->GetTeam() != 0) return;
	if (SeenCharacter == PatrolTarget) return;

	UE_LOG(LogTemp, Display, TEXT("CODE : zombie See"));
	
	PatrolTarget = SeenPawn;
	
	EnemyState = EEnemyState::EES_Chasing;
	MoveToActor(PatrolTarget, 1.5f);
	UE_LOG(LogTemp, Display, TEXT("%s"), *PatrolTarget->GetName());
	
}

void AUnrealAIController::PawnSensingHeard(APawn* HeardPawn, const FVector& Location, float Volume)
{
	if (PatrolTarget != nullptr) return;
	if (EnemyState == EEnemyState::EES_Attacking) return;
	
	const TObjectPtr<ABCharacter> HeardCharacter = Cast<ABCharacter>(HeardPawn);
	
	if (HeardCharacter == nullptr) return;
	if (HeardCharacter->GetTeam() != 0) return;
	
	UE_LOG(LogTemp, Display, TEXT("CODE : zombie HEAR"));
	
	EnemyState = EEnemyState::EES_Chasing;
	MoveToLocation(PatrolTarget->GetActorLocation(), 1.5f); // 플레이어 위치로 이동
}