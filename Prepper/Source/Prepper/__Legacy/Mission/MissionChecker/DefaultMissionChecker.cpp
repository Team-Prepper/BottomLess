// Fill out your copyright notice in the Description page of Project Settings.


#include "DefaultMissionChecker.h"

UDefaultMissionChecker::UDefaultMissionChecker(const TArray<FString>& Code)
{
}

void UDefaultMissionChecker::SetTargetMode(ASurvivorGameMode* Target)
{
	
}

bool UDefaultMissionChecker::CheckClear()
{
	return false;
}

float UDefaultMissionChecker::GetClearRate()
{
	return 0.f;
}

FString UDefaultMissionChecker::GetMissionName()
{
	return FString("Default");
}

FString UDefaultMissionChecker::GetMissionContent()
{
	return FString("Default");
}
