#include "DeathMatchPlayerController.h"

#include "EnhancedInputComponent.h"
#include "Net/UnrealNetwork.h"
#include "Components/TextBlock.h"
#include "GameFramework/GameMode.h"
#include "Kismet/GameplayStatics.h"
#include "Prepper/_DeathMatchMode/DeathMatchGameMode.h"
#include "Prepper/_DeathMatchMode/GameState/DeathMatchGameState.h"
#include "Prepper/_DeathMatchMode/UI/DeathMatchWidget.h"
#include "Prepper/_DeathMatchMode/UI/ScoreBoard.h"
#include "Prepper/___Legacy/Character/PlayerCharacter.h"
#include "Prepper/___Legacy/Character/Component/Combat/CombatComponent.h"
#include "Prepper/___Legacy/HUD/UI/Announcement.h"
#include "Prepper/___Legacy/HUD/UI/CharacterOverlay/CharacterOverlay.h"

void ADeathMatchPlayerController::BeginPlay()
{
	Super::BeginPlay();
	ServerCheckMatchState();

	if (!IsLocalController()) return;
	
	SetHUDScore(HUDScore);
	SetHUDDefeats(HUDDefeats);
}

void ADeathMatchPlayerController::BeginWidget()
{
	
	if (DeathMatchWidgetClass && DeathMatchWidget == nullptr)
	{
		DeathMatchWidget = CreateWidget<UDeathMatchWidget>(this, DeathMatchWidgetClass);
		DeathMatchWidget->AddToViewport();
	}
	
	if (AnnounceWidgetClass && AnnounceWidget == nullptr)
	{
		AnnounceWidget = CreateWidget<UAnnouncement>(this, AnnounceWidgetClass);
		AnnounceWidget->AddToViewport();
	}
	Super::BeginWidget();
	
	if (ScoreBoardClass && ScoreBoard == nullptr)
	{
		ScoreBoard = CreateWidget<UScoreBoard>(this, ScoreBoardClass);
		ScoreBoard->AddToViewport();
		ScoreBoard->SetVisibility(ESlateVisibility::Hidden);
	}
}


void ADeathMatchPlayerController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	SetHUDTime();
	CheckTimeSync(DeltaTime);
}

void ADeathMatchPlayerController::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ADeathMatchPlayerController, MatchState);
}

void ADeathMatchPlayerController::LocalPossessNewPlayerCharacter()
{
	Super::LocalPossessNewPlayerCharacter();
	
	SetHUDScore(HUDScore);
	SetHUDDefeats(HUDDefeats);

	ADeathMatchGameState* DeathMatchGameState =
		Cast<ADeathMatchGameState>(UGameplayStatics::GetGameState(this));
	ADeathMatchPlayerState* DeathMatchPlayerState = GetPlayerState<ADeathMatchPlayerState>();

	if (!DeathMatchGameState || !DeathMatchPlayerState) return;
	
	DeathMatchGameState->UpdateTopScore(DeathMatchPlayerState);
}

void ADeathMatchPlayerController::SetHUDScore(float Score)
{
	if (DeathMatchWidget)
	{
		DeathMatchWidget->SetScore(Score);
	}
	else
	{
		HUDScore = Score;
	}
}

void ADeathMatchPlayerController::SetHUDDefeats(int32 Defeats)
{
	if (DeathMatchWidget)
	{
		DeathMatchWidget->SetDefeat(Defeats);
	}
	else
	{
		HUDDefeats = Defeats;
	}
}

void ADeathMatchPlayerController::SetHUDMatchCountdown(float CountdownTime)
{
	if (!DeathMatchWidget) return;

	if (CountdownTime < 0.f)
	{
		DeathMatchWidget->SetMatchTime(-1, -1);
		return;
	}

	const int32 Minutes = FMath::FloorToInt(CountdownTime / 60.f);
	const int32 Seconds = CountdownTime - Minutes * 60;
	
	DeathMatchWidget->SetMatchTime(Minutes, Seconds);
	
}

void ADeathMatchPlayerController::SetHUDAnnouncementCountdown(float CountdownTime)
{
	if (!AnnounceWidget) return;
			
	if (CountdownTime < 0.f)
	{
		AnnounceWidget->WarmupTime->SetText(FText());
		return;
	}

	int32 Minutes = FMath::FloorToInt(CountdownTime / 60.f);
	int32 Seconds = CountdownTime - Minutes * 60;
		
	AnnounceWidget->SetWarmupTime(Minutes, Seconds);
}

void ADeathMatchPlayerController::SetHUDTime()
{
	float TimeLeft = 0.f;
	if (MatchState == MatchState::WaitingToStart) TimeLeft = WarmupTime - GetServerTime() + LevelStartingTime;
	else if (MatchState == MatchState::InProgress)
		TimeLeft = WarmupTime + MatchTime - GetServerTime() +
			LevelStartingTime;
	else if (MatchState == MatchState::Cooldown)
		TimeLeft = CooldownTime + WarmupTime + MatchTime - GetServerTime() +
			LevelStartingTime;
	uint32 SecondsLeft = FMath::CeilToInt(TimeLeft);

	if (HasAuthority())
	{
		DeathMatchGameMode = DeathMatchGameMode == nullptr
			                     ? Cast<ADeathMatchGameMode>(UGameplayStatics::GetGameMode(this))
			                     : DeathMatchGameMode;
		if (DeathMatchGameMode)
		{
			SecondsLeft = FMath::CeilToInt(DeathMatchGameMode->GetCountdownTime() + LevelStartingTime);
		}
	}

	if (CountdownInt != SecondsLeft)
	{
		if (MatchState == MatchState::WaitingToStart || MatchState == MatchState::Cooldown)
		{
			SetHUDAnnouncementCountdown(TimeLeft);
		}
		if (MatchState == MatchState::InProgress)
		{
			SetHUDMatchCountdown(TimeLeft);
		}
	}
	CountdownInt = SecondsLeft;
}

void ADeathMatchPlayerController::CheckTimeSync(float DeltaTime)
{
	TimeSyncRunningTime += DeltaTime;
	if (IsLocalController() && TimeSyncRunningTime > TimeSyncFrequency)
	{
		ServerRequestServerTime(GetWorld()->GetTimeSeconds());
		TimeSyncRunningTime = 0.f;
	}
}

float ADeathMatchPlayerController::GetServerTime()
{
	if (HasAuthority()) return GetWorld()->GetTimeSeconds();
	return GetWorld()->GetTimeSeconds() + ClientServerDelta;
}

void ADeathMatchPlayerController::ReceivedPlayer()
{
	Super::ReceivedPlayer();
	if (IsLocalController())
	{
		ServerRequestServerTime(GetWorld()->GetTimeSeconds());
	}
}

void ADeathMatchPlayerController::OnMatchStateSet(FName State)
{
	MatchState = State;

	if (MatchState == MatchState::InProgress)
	{
		HandleMatchHasStarted();
	}
	else if (MatchState == MatchState::Cooldown)
	{
		HandleCooldown();
	}
}

void ADeathMatchPlayerController::OnRep_MatchState()
{
	if (MatchState == MatchState::InProgress)
	{
		HandleMatchHasStarted();
	}
	else if (MatchState == MatchState::Cooldown)
	{
		HandleCooldown();
	}
}

void ADeathMatchPlayerController::HandleMatchHasStarted()
{
	if (AnnounceWidget)
	{
		AnnounceWidget->SetVisibility(ESlateVisibility::Hidden);
	}
}

void ADeathMatchPlayerController::HandleCooldown()
{
	
	PlayerCharacter = Cast<APlayerCharacter>(GetPawn());
	if (PlayerCharacter && PlayerCharacter->GetCombatComponent())
	{
		PlayerCharacter->bDisableGamePlay = true;
		PlayerCharacter->GetCombatComponent()->FireTrigger(false);
	}

	CharacterOverlay->RemoveFromParent();
	
	if (!AnnounceWidget) return;
	
	AnnounceWidget->SetVisibility(ESlateVisibility::Visible);
	AnnounceWidget->Announce(FString("New Match Starts In : "));

	ADeathMatchGameState* DeathMatchGameState =
		Cast<ADeathMatchGameState>(UGameplayStatics::GetGameState(this));
	const ADeathMatchPlayerState* DeathMatchPlayerState = GetPlayerState<ADeathMatchPlayerState>();
		
	if (!DeathMatchGameState || !DeathMatchPlayerState) return;

	AnnounceWidget->SetInformation(DeathMatchGameState->GetMatchResultStr(DeathMatchPlayerState));

}

void ADeathMatchPlayerController::ClientJoinMidGame_Implementation(FName StateOfMatch, float Warmup, float Match,
                                                                   float Cooldown, float StartingTime)
{
	WarmupTime = Warmup;
	MatchTime = Match;
	CooldownTime = Cooldown;
	LevelStartingTime = StartingTime;
	MatchState = StateOfMatch;
	OnMatchStateSet(MatchState);
}

void ADeathMatchPlayerController::ServerCheckMatchState_Implementation()
{
	ADeathMatchGameMode* GameMode = Cast<ADeathMatchGameMode>(UGameplayStatics::GetGameMode(this));
	if (GameMode)
	{
		WarmupTime = GameMode->WarmupTime;
		MatchTime = GameMode->MatchTime;
		CooldownTime = GameMode->CooldownTime;
		LevelStartingTime = GameMode->LevelStartingTime;
		MatchState = GameMode->GetMatchState();
		ClientJoinMidGame(MatchState, WarmupTime, MatchTime, CooldownTime, LevelStartingTime);
	}
}

void ADeathMatchPlayerController::ClientReportServerTime_Implementation(float TimeOfClientRequest,
                                                                        float TimeServerReceivedClientRequest)
{
	float RoundTripTime = GetWorld()->GetTimeSeconds() - TimeOfClientRequest;
	float CurrentServerTime = TimeServerReceivedClientRequest + (0.5f * RoundTripTime);
	ClientServerDelta = CurrentServerTime - GetWorld()->GetTimeSeconds();
}

void ADeathMatchPlayerController::ServerRequestServerTime_Implementation(float TimeOfClientRequest)
{
	float ServerTimeOfReceipt = GetWorld()->GetTimeSeconds();
	ClientReportServerTime(TimeOfClientRequest, ServerTimeOfReceipt);
}

void ADeathMatchPlayerController::SetInput(UEnhancedInputComponent* Input)
{
	Super::SetInput(Input);
	Input->BindAction(OpenInventory, ETriggerEvent::Started, this, &ADeathMatchPlayerController::OpenScoreBoard);
	Input->BindAction(OpenInventory, ETriggerEvent::Completed, this, &ADeathMatchPlayerController::CloseScoreBoard);
}

void ADeathMatchPlayerController::OpenScoreBoard()
{
	if (!ScoreBoard)
	{
		UE_LOG(LogTemp, Warning, TEXT("NO SCOREBOARD"));
		return;
	}
	ScoreBoard->SetVisibility(ESlateVisibility::Visible);
}

void ADeathMatchPlayerController::CloseScoreBoard()
{
	if (!ScoreBoard)
	{
		UE_LOG(LogTemp, Warning, TEXT("NO SCOREBOARD"));
		return;
	}
	ScoreBoard->SetVisibility(ESlateVisibility::Hidden);
}
