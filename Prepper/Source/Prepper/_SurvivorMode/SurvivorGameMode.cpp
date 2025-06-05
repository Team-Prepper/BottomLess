// Fill out your copyright notice in the Description page of Project Settings.


#include "SurvivorGameMode.h"

#include "GameSave/SurvivorServerSaveGame.h"
#include "Kismet/GameplayStatics.h"

void ASurvivorGameMode::BeginPlay()
{
	Super::BeginPlay();
	
	USurvivorServerSaveGame* LoadGameInstance =
		Cast<USurvivorServerSaveGame>(UGameplayStatics::LoadGameFromSlot(FString::Printf(TEXT("%s-%s"), *GetWorld()->GetMapName(), *FString("Server")), 0));

	if (!LoadGameInstance) return;

	if (ASurvivorGameMode* GM = GetWorld()->GetAuthGameMode<ASurvivorGameMode>())
	{
		GM->SetPlayTime(LoadGameInstance->PlayTime);
		for (auto Str : LoadGameInstance->Achievement)
		{
			GM->AddAchievement(Str.Key, Str.Value);
		}
	}
}

void ASurvivorGameMode::PlayerEliminated(ABaseCharacter* ElimmedCharacter, ABasePlayerController* VictimController,
                                         ABasePlayerController* AttackerController)
{
	Super::PlayerEliminated(ElimmedCharacter, VictimController, AttackerController);
}

void ASurvivorGameMode::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	PlayTime += DeltaSeconds;
}

void ASurvivorGameMode::AddMission()
{
	
}

float ASurvivorGameMode::GetPlayTime() const
{
	return PlayTime;
}

void ASurvivorGameMode::SetPlayTime(const float Time)
{
	PlayTime = Time;
}

void ASurvivorGameMode::AddAchievement(const FString& NewAchievement, const bool Value)
{
	Achievement.Add(NewAchievement, Value);

	UE_LOG(LogTemp, Warning, TEXT("Acheivement: %s"), *NewAchievement);
}

bool ASurvivorGameMode::IsAchieved(const FString& TargetAchievement)
{
	if (!Achievement.Contains(TargetAchievement)) return false;
	return Achievement[TargetAchievement];
}
