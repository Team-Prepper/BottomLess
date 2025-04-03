// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PrepperGameMode.h"
#include "Prepper/__Legacy/GameSave/SurvivorServerSaveGame.h"
#include "Prepper/__Legacy/Mission/Mission.h"
#include "UObject/Object.h"
#include "SurvivorGameMode.generated.h"

class APlayerCharacter;
/**
 * 
 */
UCLASS()
class PREPPER_API ASurvivorGameMode : public APrepperGameMode
{
	GENERATED_BODY()
	friend class UBaseCombatComponent;
	friend class UCombatComponent;
private:
	int EnemyKillCount;
	float PlayTime;
	TMap<FString, bool> Achievement;
public:
	virtual void PlayerEliminated(ABaseCharacter* ElimmedCharacter,
									ABasePlayerController* VictimController,
									ABasePlayerController* AttackerController) override;
	virtual void Tick(float DeltaSeconds) override;
	void AddMission();
	int GetEnemyKillCount() const { return EnemyKillCount; }
	
	float GetPlayTime() const;
	void SetPlayTime(float Time);
	void AddAchievement(const FString &NewAchievement, bool Value = true);
	bool IsAchieved(const FString &Achievement);
	TMap<FString, bool> GetAchievement() { return Achievement; }
};
