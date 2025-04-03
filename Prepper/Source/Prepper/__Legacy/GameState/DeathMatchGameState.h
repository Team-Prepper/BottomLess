// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <set>

#include "CoreMinimal.h"
#include "GameFramework/GameState.h"
#include "Prepper/_Base/ObserverPattern/Subject.h"
#include "DeathMatchGameState.generated.h"

class ADeathMatchPlayerState;

UCLASS()
class PREPPER_API ADeathMatchGameState : public AGameState, public ISubject<TArray<TObjectPtr<ADeathMatchPlayerState>>>
{
	GENERATED_BODY()

	float TopScore = 0.f;

	TArray<TObjectPtr<ADeathMatchPlayerState>> Players;
	UPROPERTY(Replicated)
	TArray<ADeathMatchPlayerState*> TopScoringPlayers;
	
	UFUNCTION()
	void OnRep_Player();
public:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>&) const override;
	virtual void AddPlayerState(APlayerState* PlayerState) override;
	void UpdateTopScore(ADeathMatchPlayerState* ScoringPlayer);

	TArray<TObjectPtr<ADeathMatchPlayerState>> GetAllPlayerState() const { return Players; }
	FString GetMatchResultStr(const ADeathMatchPlayerState* Player);
	
private:
	std::pmr::set<IObserver<TArray<TObjectPtr<ADeathMatchPlayerState>>>*> Observers;

public:
	virtual void Attach(IObserver<TArray<TObjectPtr<ADeathMatchPlayerState>>>* Observer) override;
	virtual void Detach(IObserver<TArray<TObjectPtr<ADeathMatchPlayerState>>>* Observer) override;
	virtual void Notify();
private:
	
};
