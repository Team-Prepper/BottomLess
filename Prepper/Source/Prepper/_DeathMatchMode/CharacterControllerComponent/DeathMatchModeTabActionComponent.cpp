// Fill out your copyright notice in the Description page of Project Settings.


#include "DeathMatchModeTabActionComponent.h"

#include "Blueprint/UserWidget.h"
#include "Prepper/_DeathMatchMode/UI/ScoreBoard.h"

UDeathMatchModeTabActionComponent::UDeathMatchModeTabActionComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UDeathMatchModeTabActionComponent::TabPressed()
{
	if (ScoreBoardClass != nullptr && ScoreBoard == nullptr)
	{
		ScoreBoard = CreateWidget<UScoreBoard>(GetOwner<APlayerController>(), ScoreBoardClass);
		ScoreBoard->AddToViewport();
	}
	if (ScoreBoard == nullptr) return;
	
	ScoreBoard->SetVisibility(ESlateVisibility::Visible);
}

void UDeathMatchModeTabActionComponent::TabReleased()
{
	if (!ScoreBoard)
	{
		UE_LOG(LogTemp, Warning, TEXT("NO SCOREBOARD"));
		return;
	}
	ScoreBoard->SetVisibility(ESlateVisibility::Hidden);
}
