#include "DeathMatchGameMode.h"

#include "GameFramework/Character.h"
#include "Prepper/_DeathMatchMode/GameState/DeathMatchGameState.h"
#include "Prepper/_DeathMatchMode/PlayerState/DeathMatchPlayerState.h"
#include "Prepper/___Legacy/Character/BaseCharacter.h"
#include "Prepper/GamePlay/Character/BCharacter.h"
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
	ScoreCalc(VictimController, AttackerController);
	AddRespawnQueue(ElimmedCharacter, VictimController);
	
	Super::PlayerEliminated(ElimmedCharacter, VictimController, AttackerController);
}

void ADeathMatchGameMode::PlayerEliminated(ABCharacter* ElimmedCharacter, AController* VictimController,
	AController* AttackerController)
{
	ScoreCalc(VictimController, AttackerController);
	AddRespawnQueue(ElimmedCharacter, VictimController);
	
	Super::PlayerEliminated(ElimmedCharacter, VictimController, AttackerController);
}

void ADeathMatchGameMode::ScoreCalc(const TObjectPtr<AController> VictimController,
                                    const TObjectPtr<AController> AttackerController) const
{
	ADeathMatchPlayerState* AttackPlayerState =
		AttackerController ? Cast<ADeathMatchPlayerState>(AttackerController->PlayerState) : nullptr;
	ADeathMatchPlayerState* VictimPlayerState =
		VictimController ? Cast<ADeathMatchPlayerState>(VictimController->PlayerState) : nullptr;

	UE_LOG(LogTemp, Warning, TEXT("Score Good"));
	if (ADeathMatchGameState* DeathMatchGameState = GetGameState<ADeathMatchGameState>(); AttackPlayerState && AttackPlayerState != VictimPlayerState && DeathMatchGameState)
	{
		AttackPlayerState->AddToScore(1.0f);
		DeathMatchGameState->UpdateTopScore(AttackPlayerState);
	}
	if (VictimPlayerState)
	{
		VictimPlayerState->AddToDefeats(1);
	}
}

void ADeathMatchGameMode::AddRespawnQueue(const TObjectPtr<ACharacter> Character, const TObjectPtr<AController> Controller)
{
	UE_LOG(LogTemp, Warning, TEXT("Respawn Queue"));
	RequestQueue.Add(FRequestQueueUnit(Character, Controller));
	
	FTimerHandle TestTimeHandle;
	GetWorld()->GetTimerManager().SetTimer(
		TestTimeHandle, this, &ADeathMatchGameMode::Respawn, RespawnTime);
	
}

void ADeathMatchGameMode::Respawn()
{
	if (!HasAuthority()) return;
	
	RequestRespawn(RequestQueue[0].Character, RequestQueue[0].Controller);
	RequestQueue.RemoveAt(0);
	
}