// Fill out your copyright notice in the Description page of Project Settings.


#include "DeathMatchWidget.h"

#include "Components/TextBlock.h"

void UDeathMatchWidget::SetMatchTime(int Minutes, int Seconds)
{
	if (Minutes < 0)
	{
		MatchTime->SetText(FText());
		return;
	}
	
	MatchTime->SetText(FText::FromString(
		FString::Printf(TEXT("%02d:%02d"), Minutes, Seconds)));
}

void UDeathMatchWidget::SetScore(float Score) const
{
	FString ScoreText = FString::Printf(TEXT("%d"),FMath::FloorToInt(Score));
	ScoreValue->SetText(FText::FromString(ScoreText));
}

void UDeathMatchWidget::SetDefeat(int Defeat) const
{
	FString DefeatsText = FString::Printf(TEXT("%d"), Defeat);
	DefeatsValue->SetText(FText::FromString(DefeatsText));
}
