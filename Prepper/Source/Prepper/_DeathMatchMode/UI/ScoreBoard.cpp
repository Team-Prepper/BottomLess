// Fill out your copyright notice in the Description page of Project Settings.


#include "ScoreBoard.h"

#include "ScoreBoardUnitData.h"
#include "Components/ListView.h"
#include "Kismet/GameplayStatics.h"
#include "Prepper/GamePlay/PlayerState/DeathMatchPlayerState.h"
#include "Prepper/_DeathMatchMode/GameState/DeathMatchGameState.h"

void UScoreBoard::SetVisibility(ESlateVisibility InVisibility)
{
	Super::SetVisibility(InVisibility);

	ADeathMatchGameState* DeathMatchGameState =
		Cast<ADeathMatchGameState>(UGameplayStatics::GetGameState(this));

	if (!DeathMatchGameState) return;
	
	if (InVisibility != ESlateVisibility::Visible)
	{
		DeathMatchGameState->Detach(this);
		return;
	}

	DeathMatchGameState->Attach(this);

}

void UScoreBoard::Update(const TArray<TObjectPtr<ADeathMatchPlayerState>>& newData)
{
	Ranking->ClearListItems();
	
	for (int i = 0; i < newData.Num(); i++)
	{
		UScoreBoardUnitData* Data =
			NewObject<UScoreBoardUnitData>(GetWorld(), UScoreBoardUnitData::StaticClass());

		Data->PlayerName = newData[i]->GetPlayerName();
		Data->Score = newData[i]->GetScore();
		Data->Defeat = newData[i]->GetDefeat();
		
		Ranking->AddItem(Data);
	}
}
