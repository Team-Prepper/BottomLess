#include "DeathMatchGameMode.h"

#include "GameFramework/Character.h"
#include "Prepper/_DeathMatchMode/GameState/DeathMatchGameState.h"
#include "Prepper/GamePlay/PlayerState/DeathMatchPlayerState.h"
#include "Prepper/___Legacy/Character/BaseCharacter.h"
#include "Prepper/___Legacy/PlayerController/DeathMatchPlayerController.h"

namespace MatchState
{
	const FName Cooldown = FName("Cooldown");
}

ADeathMatchGameMode::ADeathMatchGameMode()
{
	bDelayedStart = true;
}

void ADeathMatchGameMode::BeginPlay()
{
	Super::BeginPlay();

	LevelStartingTime = GetWorld()->GetTimeSeconds();
}

void ADeathMatchGameMode::OnMatchStateSet()
{
	Super::OnMatchStateSet();

	if (!GetWorld()->GetPlayerControllerIterator()) return;
	
	for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
	{
		ADeathMatchPlayerController* PrepperController = Cast<ADeathMatchPlayerController>(*It);
		if (PrepperController)
		{
			PrepperController->OnMatchStateSet(MatchState);
		}
	}
}

void ADeathMatchGameMode::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	// 한 번만 호출하여 현재 시간을 가져옵니다.
	float CurrentTime = GetWorld()->GetTimeSeconds();

	if (MatchState == MatchState::WaitingToStart)
	{
		CountdownTime = WarmupTime - CurrentTime + LevelStartingTime;
		if (CountdownTime <= 0.f)
		{
			StartMatch();
		}
		return;
	}
	if (MatchState == MatchState::InProgress)
	{
		CountdownTime = WarmupTime + MatchTime - CurrentTime + LevelStartingTime;
		if (CountdownTime <= 0.f)
		{
			SetMatchState(MatchState::Cooldown);
		}
		return;
	}

	CountdownTime = CooldownTime + WarmupTime + MatchTime - CurrentTime + LevelStartingTime;
	if (CountdownTime <= 0.f)
	{
		RestartGame();
	}
}

void ADeathMatchGameMode::PlayerEliminated(ABaseCharacter* ElimmedCharacter,
                                           ABasePlayerController* VictimController, ABasePlayerController* AttackerController)
{
	/* for Score */
	ADeathMatchPlayerState* AttackPlayerState = AttackerController ? Cast<ADeathMatchPlayerState>(AttackerController->PlayerState) : nullptr;
	ADeathMatchPlayerState* VictimPlayerState = VictimController ? Cast<ADeathMatchPlayerState>(VictimController->PlayerState) : nullptr;

	ADeathMatchGameState* DeathMatchGameState = GetGameState<ADeathMatchGameState>();
	
	if (AttackPlayerState && AttackPlayerState != VictimPlayerState && DeathMatchGameState)
	{
		AttackPlayerState->AddToScore(1.0f);
		DeathMatchGameState->UpdateTopScore(AttackPlayerState);
	}
	if (VictimPlayerState)
	{
		VictimPlayerState->AddToDefeats(1);
	}

	RequestQueue.Add(FRequestQueueUnit(ElimmedCharacter, VictimController));
	
	FTimerHandle TestTimeHandle;
	GetWorld()->GetTimerManager().SetTimer(
		TestTimeHandle, this, &ADeathMatchGameMode::Respawn, RespawnTime);
	
	Super::PlayerEliminated(ElimmedCharacter, VictimController, AttackerController);
}

void ADeathMatchGameMode::Respawn()
{
	ADeathMatchGameMode* DeathMatchGameMode = GetWorld()->GetAuthGameMode<ADeathMatchGameMode>();

	if (!DeathMatchGameMode) return;
	
	DeathMatchGameMode->RequestRespawn(RequestQueue[0].Character, RequestQueue[0].Controller);
	RequestQueue.RemoveAt(0);
}