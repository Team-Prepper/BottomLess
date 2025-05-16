// Fill out your copyright notice in the Description page of Project Settings.


#include "UnrealDeathMatchModeTabActionComponent.h"

#include "Blueprint/UserWidget.h"
#include "Prepper/__Legacy/HUD/UI/DeathMatch/ScoreBoard.h"

UUnrealDeathMatchModeTabActionComponent::UUnrealDeathMatchModeTabActionComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UUnrealDeathMatchModeTabActionComponent::TabPressed()
{
	if (ScoreBoardClass != nullptr && ScoreBoard == nullptr)
	{
		ScoreBoard = CreateWidget<UScoreBoard>(GetOwner<APlayerController>(), ScoreBoardClass);
		ScoreBoard->AddToViewport();
	}
	if (ScoreBoard == nullptr) return;
	
	ScoreBoard->SetVisibility(ESlateVisibility::Visible);
}

void UUnrealDeathMatchModeTabActionComponent::TabReleased()
{
	if (!ScoreBoard)
	{
		UE_LOG(LogTemp, Warning, TEXT("NO SCOREBOARD"));
		return;
	}
	ScoreBoard->SetVisibility(ESlateVisibility::Hidden);
}
