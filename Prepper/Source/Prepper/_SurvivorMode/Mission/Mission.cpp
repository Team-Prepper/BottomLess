// Fill out your copyright notice in the Description page of Project Settings.


#include "Mission.h"

#include "Kismet/GameplayStatics.h"
#include "MissionChecker/DefaultMissionChecker.h"
#include "Prepper/_SurvivorMode/SurvivorGameMode.h"

TArray<FString> UMission::SplitString(const FString& Str)
{
	TArray<FString> CodePiece; 
	const TCHAR* Delims[] = { TEXT(":") };
	const int32 ArraySize = Str.ParseIntoArray(CodePiece, Delims, 1);
	
	return CodePiece;
}

IMissionChecker* UMission::GetMissionChecker(const FString& Code)
{
	TArray<FString> CodePiece = SplitString(Code);
	
	return new UDefaultMissionChecker(CodePiece);
}

IMissionReward* UMission::GetMissionReward(const FString& Code)
{
	TArray<FString> CodePiece = SplitString(Code);

	return nullptr;
}

UMission::UMission()
{
	Checker = nullptr;
	Reward = nullptr;
}

UMission::UMission(const FString& CheckerCode, const FString& RewardCode)
{
	Checker = GetMissionChecker(CheckerCode);
	Checker->SetTargetMode(Cast<ASurvivorGameMode>(UGameplayStatics::GetGameMode(this)));
	
	Reward = GetMissionReward(RewardCode);
}
