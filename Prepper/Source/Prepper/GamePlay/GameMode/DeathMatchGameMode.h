// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "PrepperGameMode.h"
#include "DeathMatchGameMode.generated.h"

namespace MatchState
{
	extern PREPPER_API const FName Cooldown; // Match duration has been reached. Display winner and begin cooldown timer.
}

struct FRequestQueueUnit
{
	ACharacter* Character;
	AController* Controller;
	FRequestQueueUnit(ACharacter* TargetCharacter, AController* TargetController)
	{
		Character = TargetCharacter;
		Controller = TargetController;
	}
};

UCLASS()
class PREPPER_API ADeathMatchGameMode : public APrepperGameMode
{
	GENERATED_BODY()
private:
	TArray<FRequestQueueUnit> RequestQueue;
public:
	ADeathMatchGameMode();
	virtual void Tick(float DeltaSeconds) override;
	virtual void PlayerEliminated(ABaseCharacter* ElimmedCharacter,
									ABasePlayerController* VictimController,
									ABasePlayerController* AttackerController) override;
	
	void Respawn();
	
	UPROPERTY(EditDefaultsOnly)
	float WarmupTime = 10.f;
	
	UPROPERTY(EditDefaultsOnly)
	float MatchTime = 120.f;

	UPROPERTY(EditDefaultsOnly)
	float CooldownTime = 10.f;

	UPROPERTY(EditDefaultsOnly)
	float RespawnTime = 3.f;
	
	float LevelStartingTime = 0.f;

protected:
	virtual void BeginPlay() override;
	virtual void OnMatchStateSet() override;

private:
	float CountdownTime = 0.f;

public:
	FORCEINLINE float GetCountdownTime() const {return CountdownTime; }
};
