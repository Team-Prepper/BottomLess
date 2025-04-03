#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "Prepper/__Legacy/ControlMapper/ControlMapper.h"
#include "BasePlayerController.generated.h"

class UInputAction;
class UCharacterOverlay;
class UCompass;
class APrepperHUD;
class APlayerCharacter;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FHighPingDelegate, bool, bPingTooHigh);

UCLASS()
class PREPPER_API ABasePlayerController : public APlayerController
{
	GENERATED_BODY()

protected:

	UPROPERTY(EditAnywhere, Category = "Player HUD")
	TSubclassOf<UCharacterOverlay> CharacterOverlayClass;
	UPROPERTY()
	TObjectPtr<UCharacterOverlay> CharacterOverlay;

	UPROPERTY(EditAnywhere, Category = "Player HUD")
	TSubclassOf<UCompass> CompassHUDClass;
	UPROPERTY()
	TObjectPtr<UCompass> Compass;
	
	UPROPERTY(EditAnywhere, Category = "Player HUD")
	TSubclassOf<UUserWidget> SettingClass;
	UPROPERTY()
	TObjectPtr<UUserWidget> Setting;
	
	UPROPERTY(EditAnywhere, Category = "Player HUD")
	TSubclassOf<UUserWidget> DeathWidgetClass;
	UPROPERTY()
	TObjectPtr<UUserWidget> DeathWidget;

	UPROPERTY()
	APrepperHUD* PrepperHUD;
	
	UPROPERTY()
	APlayerCharacter* PlayerCharacter;
	
public:
	virtual void Tick(float DeltaTime) override;
	virtual void SetPawn(APawn* InPawn) override;
	virtual void OnPossess(APawn* InPawn) override;
	virtual void OnRep_Pawn() override;
	virtual void PlayerTick(float DeltaTime) override;
	TObjectPtr<APlayerCharacter> GetPlayerCharacter();
	
protected:
	virtual void BeginPlay() override;
	virtual void BeginWidget();
	virtual void ServerPossessNewPlayerCharacter();
	virtual void LocalPossessNewPlayerCharacter();
	
	APrepperHUD* GetPrepperHUD();
	
public:
	/* NetWork */
	FHighPingDelegate HighPingDelegate;
	void Elim();
	UFUNCTION(NetMulticast, Reliable)
	void MulticastElim();
	UFUNCTION(Server, Reliable)
	void ServerReportPingStatus(bool bHighPing);

	/* HIGH PING WARNING */
	float HighPingRunningTime = 0.f;
	float PingAnimationRunningTime = 5.f;
	
	UPROPERTY(EditAnywhere)
	float HighPingDuration = 5.f;
	UPROPERTY(EditAnywhere)
	float CheckPingFrequency = 10.f;
	UPROPERTY(EditAnywhere)
	float HighPingThreshold = 50.f;
	
	void CheckPing(float DeltaTime);

	UFUNCTION(BlueprintImplementableEvent)
	void HighPingWarningBP();

	UFUNCTION(BlueprintImplementableEvent)
	void StopHighPingWarningBP();

	/* Input Component */
public:
	virtual void SetupInputComponent() override;

	IControlMapper* TargetControlMapper = nullptr;
protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = true))
	class UInputMappingContext* PlayerMappingContext;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = true))
	UInputAction* MoveAction;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = true))
	UInputAction* JumpAction;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = true))
	UInputAction* LookAction;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = true))
	UInputAction* SprintAction;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = true))
	UInputAction* EquipAction;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = true))
	UInputAction* CrouchAction;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = true))
	UInputAction* ReloadAction;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = true))
	UInputAction* AimAction;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = true))
	UInputAction* FireAction;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = true))
	UInputAction* OpenInventory;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = true))
	UInputAction* OpenSetting;

	virtual void SetInput(UEnhancedInputComponent* Input);
	
	void Move(const FInputActionValue& Value);
	void Look(const FInputActionValue& Value);
	void JumpButtonPressed();
	void JumpButtonReleased();
	void SprintButtonPressed();
	void SprintButtonReleased();
	void EquipButtonPressed();
	void CrouchButtonPressed();
	void ReloadButtonPressed();
	void AimButtonPressed();
	void AimButtonReleased();
	void FireButtonPressed();
	void FireButtonReleased();
	void OpenSettingWidget();
	void CloseSettingWidget();

	UFUNCTION(Server, Reliable)
	void ServerInteractionPressed();

};
