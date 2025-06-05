#pragma once

#include "BasePlayerController.h"
#include "DeathMatchPlayerController.generated.h"

class UAnnouncement;
class UDeathMatchWidget;
class UScoreBoard;
UCLASS()
class PREPPER_API ADeathMatchPlayerController : public ABasePlayerController
{
	GENERATED_BODY()
private:
	UPROPERTY(EditAnywhere, Category = "Player HUD")
	TSubclassOf<UScoreBoard> ScoreBoardClass;
	UPROPERTY()
	TObjectPtr<UScoreBoard> ScoreBoard;
	
	UPROPERTY(EditAnywhere, Category = "Player HUD")
	TSubclassOf<UDeathMatchWidget> DeathMatchWidgetClass;
	UPROPERTY()
	TObjectPtr<UDeathMatchWidget> DeathMatchWidget;

	UPROPERTY(EditAnywhere, Category = "Player HUD")
	TSubclassOf<UAnnouncement> AnnounceWidgetClass;
	UPROPERTY()
	TObjectPtr<UAnnouncement> AnnounceWidget;
	
protected:
	virtual void BeginPlay() override;
	virtual void BeginWidget() override;
public:
	virtual void Tick(float DeltaTime) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void LocalPossessNewPlayerCharacter() override;

	void SetHUDScore(float Score);
	void SetHUDDefeats(int32 Defeats);
	void SetHUDMatchCountdown(float CountdownTime);
	void SetHUDAnnouncementCountdown(float CountdownTime);

	void SetHUDTime();

	UPROPERTY()
	class ADeathMatchGameMode* DeathMatchGameMode;

	// play time counter (in game)
	float LevelStartingTime = 0.f;
	float MatchTime = 0.f;
	float WarmupTime = 0.f;
	float CooldownTime = 0.f;
	uint32 CountdownInt = 0;

	UFUNCTION(Server, Reliable)
	void ServerRequestServerTime(float TimeOfClientRequest);
	UFUNCTION(Client, Reliable)
	void ClientReportServerTime(float TimeOfClientRequest, float TimeServerReceivedClientRequest);
	
	float ClientServerDelta = 0.f; // difference between client and server time

	UPROPERTY(EditAnywhere, Category = Time)
	float TimeSyncFrequency = 5.f;

	float TimeSyncRunningTime = 0.f;
	void CheckTimeSync(float DeltaTime);

	/* Sync time client <-> server*/
public:
	virtual float GetServerTime(); // Synced with server world clock
	virtual void ReceivedPlayer() override; // Sync with server clock as soon as possible

	/* Match Mode */
	UPROPERTY(ReplicatedUsing = OnRep_MatchState)
	FName MatchState;
	
	void OnMatchStateSet(FName State);
	UFUNCTION()
	void OnRep_MatchState();

	float HUDScore;
	int32 HUDDefeats;

		
protected:
	void HandleMatchHasStarted();
	void HandleCooldown();
	UFUNCTION(Server, Reliable)
	void ServerCheckMatchState();

	UFUNCTION(Client, Reliable)
	void ClientJoinMidGame(FName StateOfMatch, float Warmup, float Match, float Cooldown, float StartingTime);

protected:
	virtual void SetInput(UEnhancedInputComponent* Input) override;
	void OpenScoreBoard();
	void CloseScoreBoard();
};
