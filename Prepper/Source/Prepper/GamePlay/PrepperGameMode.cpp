// Fill out your copyright notice in the Description page of Project Settings.


#include "PrepperGameMode.h"

#include "GameFramework/PlayerStart.h"
#include "Kismet/GameplayStatics.h"
#include "Prepper/___Legacy/Character/BaseCharacter.h"
#include "Prepper/___Legacy/PlayerController/BasePlayerController.h"

void APrepperGameMode::PlayerEliminated(ABaseCharacter* ElimmedCharacter, ABasePlayerController* VictimController,
                                        ABasePlayerController* AttackerController)
{
	PlayerEliminated(ElimmedCharacter);

	if (VictimController) VictimController->Elim();
}

void APrepperGameMode::PlayerEliminated(ABCharacter* ElimmedCharacter)
{
	
}

void APrepperGameMode::PlayerEliminated(ABCharacter* ElimmedCharacter, AController* VictimController,
	AController* AttackerController)
{
	
}

void APrepperGameMode::RequestRespawn(ACharacter* ElimmedCharacter, AController* ElimmedController)

{
	if (ElimmedCharacter)
	{
		ElimmedCharacter->Reset();
		ElimmedCharacter->Destroy();
	}
	if (!ElimmedController) return;
	
	TArray<AActor*> PlayerStarts;
	UGameplayStatics::GetAllActorsOfClass(this, APlayerStart::StaticClass(), PlayerStarts);
	
	const int32 Selection = FMath::RandRange(0, PlayerStarts.Num() - 1);
	RestartPlayerAtPlayerStart(ElimmedController, PlayerStarts[Selection]);
	
}

void APrepperGameMode::PlayerEliminated(ABaseCharacter* ElimmedCharacter)
{
	if (ElimmedCharacter)
	{
		ElimmedCharacter->Elim();
		UE_LOG(LogTemp, Display, TEXT("Elim Character"));
	}
}
