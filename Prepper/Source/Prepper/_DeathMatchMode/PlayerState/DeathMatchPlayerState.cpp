// Fill out your copyright notice in the Description page of Project Settings.


#include "DeathMatchPlayerState.h"

#include "Prepper/___Legacy/Character/PlayerCharacter.h"
#include "Net/UnrealNetwork.h"

void ADeathMatchPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ADeathMatchPlayerState, Defeats);
}


void ADeathMatchPlayerState::AddToScore(float ScoreValue)
{
	SetScore(GetScore() + ScoreValue);
	Notify();
}

void ADeathMatchPlayerState::AddToDefeats(int32 DefeatsValue)
{
	Defeats += DefeatsValue;
	Notify();
}

void ADeathMatchPlayerState::Attach(IObserver<ADeathMatchPlayerState>* Observer)
{
	Observers.Add(Observer);;
	Observer->Update(*this);
}

void ADeathMatchPlayerState::Detach(IObserver<ADeathMatchPlayerState>* Observer)
{
	Observers.Remove(Observer);
}

void ADeathMatchPlayerState::Notify()
{
	for(const auto Observer : Observers)
	{
		Observer->Update(*this);
	}
}

void ADeathMatchPlayerState::OnRep_Defeats()
{
	Notify();
}

void ADeathMatchPlayerState::OnRep_Score()
{
	Super::OnRep_Score();
	Notify();
}



