// Fill out your copyright notice in the Description page of Project Settings.


#include "UnrealAIController.h"
#include "UnrealStatusComponent.h"
#include "UnrealCombatComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Perception/PawnSensingComponent.h"
#include "Prepper/GamePlay/Character/BCharacter.h"
#include "Prepper/__Legacy/Character/EnemyBaseCharacter.h"

void AUnrealAIController::BeginPlay()
{
	if (!GetTargetCharacter()) return;
	
	GetTargetCharacter()->GetPawnSensing()->SetComponentTickEnabled(true);
	GetTargetCharacter()->GetPawnSensing()->SightRadius = 4000.f;
	GetTargetCharacter()->GetPawnSensing()->SetPeripheralVisionAngle(100.f);
	
	GetTargetCharacter()->GetPawnSensing()->OnSeePawn.AddDynamic(this, &AUnrealAIController::PawnSensingSeen);
	GetTargetCharacter()->GetPawnSensing()->OnHearNoise.AddDynamic(this, &AUnrealAIController::PawnSensingHearn);
}

void AUnrealAIController::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	
	// 공격사거리 안에서 공격이 아닐떄 -> 공격!
	if (InTargetRange(PatrolTarget, AttackRadius))
	{
		GetTargetCharacter()->GetCharacterMovement()->StopMovementImmediately();
		//UE_LOG(LogTemp, Warning, TEXT("CODE : zombie Attack"));
		Combat->AttackTrigger(true);

		//UE_LOG(LogTemp, Warning, TEXT("%hs"), CombatComp == nullptr ? "True":"False");
		return;
	}
	
	Combat->AttackTrigger(false);

	if (InTargetRange(PatrolTarget, CombatRadius))
	{
		//UE_LOG(LogTemp, Warning, TEXT("Enemy Found Target -> chasing"));
		EnemyState = EEnemyState::EES_Chasing;
		GetTargetCharacter()->SprintTrigger(true);
		MoveToActor(PatrolTarget);
		return;
	}

	if (PatrolTarget != nullptr)
	{
		//UE_LOG(LogTemp, Warning, TEXT("Lost Target"));
		EnemyState = EEnemyState::EES_Patrolling;
		GetTargetCharacter()->SprintTrigger(false);
		MoveToActor(PatrolTarget);
	}
	
}

bool AUnrealAIController::InTargetRange(const TObjectPtr<AActor> Target, const float Radius)
{
	if (Target == nullptr) return false;

	const float DistanceToTarget = (Target->GetActorLocation() - GetTargetCharacter()->GetActorLocation()).SizeSquared();
	return DistanceToTarget <= Radius * Radius;
}


TObjectPtr<ABCharacter> AUnrealAIController::GetTargetCharacter()
{
	if (TargetCharacter == nullptr)
	{
		TargetCharacter = GetPawn<ABCharacter>();
	}
	return TargetCharacter;
}

IStatus* AUnrealAIController::GetStatus()
{
	return Status;
}

ICombat* AUnrealAIController::GetCombat()
{
	return Combat;
}

void AUnrealAIController::Move(const FInputActionValue& Value)
{
	
}

void AUnrealAIController::Look(const FInputActionValue& Value)
{
	
}

void AUnrealAIController::CrouchToggle()
{
}

void AUnrealAIController::JumpTrigger(bool IsTrigger)
{
}

void AUnrealAIController::SprintTrigger(bool IsTrigger)
{
}

void AUnrealAIController::EquipButtonPressed()
{
}

void AUnrealAIController::PawnSensingSeen(APawn* SeenPawn)
{
	PatrolTarget = SeenPawn;
	
	if (EnemyState != EEnemyState::EES_Attacking)
	{
		EnemyState = EEnemyState::EES_Chasing;
		FVector TargetLocation = PatrolTarget->GetActorLocation(); // 플레이어의 위치를 복사하여 전달
		MoveToLocation(TargetLocation); // 플레이어 위치로 이동
	}
	
}

void AUnrealAIController::PawnSensingHearn(APawn* HearnPawn, const FVector& Location, float Volume)
{
	if (EnemyState == EEnemyState::EES_Chasing) return;
	
	UE_LOG(LogTemp, Display, TEXT("CODE : zombie HEAR"));
	
	PatrolTarget = HearnPawn;
	
	if (EnemyState != EEnemyState::EES_Attacking)
	{
		EnemyState = EEnemyState::EES_Chasing;
		FVector TargetLocation = PatrolTarget->GetActorLocation(); // 플레이어의 위치를 복사하여 전달
		MoveToLocation(TargetLocation); // 플레이어 위치로 이동
	}
}