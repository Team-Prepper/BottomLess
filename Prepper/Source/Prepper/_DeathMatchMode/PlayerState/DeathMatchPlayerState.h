// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "Prepper/__Base/ObserverPattern/Subject.h"
#include "DeathMatchPlayerState.generated.h"

/**
 * 
 */
UCLASS()
class PREPPER_API ADeathMatchPlayerState : public APlayerState, public ISubject<ADeathMatchPlayerState>
{
	GENERATED_BODY()
	
private:
	UPROPERTY()
	class APlayerCharacter* Character;
	UPROPERTY()
	class ADeathMatchPlayerController* Controller;
	
	UPROPERTY(ReplicatedUsing = OnRep_Defeats)
	int32 Defeats;
	
	TSet<IObserver<ADeathMatchPlayerState>*> Observers;
	
public:
	virtual void  GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	// REPLICATION NOTIFY
	virtual void OnRep_Score() override;
	
	void AddToScore(float ScoreValue);
	int GetDefeat() const {return Defeats;}

	virtual void Attach(IObserver<ADeathMatchPlayerState>* Observer) override;
	virtual void Detach(IObserver<ADeathMatchPlayerState>* Observer) override;

	void Notify();
	
	UFUNCTION()
	virtual void OnRep_Defeats();
	void AddToDefeats(int32 DefeatsValue);
	
};
