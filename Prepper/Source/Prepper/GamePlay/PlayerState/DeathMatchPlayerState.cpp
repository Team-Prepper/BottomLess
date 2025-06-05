// Fill out your copyright notice in the Description page of Project Settings.


#include "DeathMatchPlayerState.h"

#include "Prepper/___Legacy/Character/PlayerCharacter.h"
#include "Prepper/___Legacy/PlayerController/DeathMatchPlayerController.h"
#include "Net/UnrealNetwork.h"

void ADeathMatchPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ADeathMatchPlayerState, Defeats);
}


void ADeathMatchPlayerState::AddToScore(float ScoreValue)
{
	SetScore(GetScore() + ScoreValue);
	Character = Character == nullptr ? Cast<APlayerCharacter>(GetPawn()) : Character;
	if(Character)
	{
		Controller = Controller == nullptr ? Cast<ADeathMatchPlayerController>(Character->Controller) : Controller;
		if(Controller)
		{
			Controller->SetHUDScore(GetScore());
		}
	}
}

void ADeathMatchPlayerState::AddToDefeats(int32 DefeatsValue)
{
	Defeats += DefeatsValue;
	Character = Character == nullptr ? Cast<APlayerCharacter>(GetPawn()) : Character;
	if(Character)
	{
		Controller = Controller == nullptr ? Cast<ADeathMatchPlayerController>(Character->Controller) : Controller;
		if(Controller)
		{
			Controller->SetHUDDefeats(Defeats);
		}
	}
}

void ADeathMatchPlayerState::OnRep_Defeats()
{
	Character = Character == nullptr ? Cast<APlayerCharacter>(GetPawn()) : Character;
	if(Character)
	{
		Controller = Controller == nullptr ? Cast<ADeathMatchPlayerController>(Character->Controller) : Controller;
		if(Controller)
		{
			Controller->SetHUDDefeats(Defeats);
		}
	}
}

void ADeathMatchPlayerState::OnRep_Score()
{
	Super::OnRep_Score();

	Character = Character == nullptr ? Cast<APlayerCharacter>(GetPawn()) : Character;
	if(Character)
	{
		Controller = Controller == nullptr ? Cast<ADeathMatchPlayerController>(Character->Controller) : Controller;
		if(Controller)
		{
			Controller->SetHUDScore(GetScore());
		}
	}
}



