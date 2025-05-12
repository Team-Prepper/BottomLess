// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "Prepper/GamePlay/CharacterController/CharacterController.h"
#include "UnrealPlayerController.generated.h"

class UWeaponWidget;
class UUnrealTabActionComponent;
class UUnrealCombatComponent;
class UUnrealStatusComponent;
class UUnrealPlayerInputComponent;
class UUnrealInteractionComponent;
class IControllerMapper;
class ABCharacter;

/**
 * 
 */

class UPlayerOverlay;

UCLASS()
class PREPPER_API AUnrealPlayerController : public APlayerController
{
	GENERATED_BODY()

	TObjectPtr<ABCharacter> TargetCharacter;
	
	UPROPERTY(EditAnywhere, Category = "Player HUD")
	TSubclassOf<UPlayerOverlay>  PlayerOverlayClass;
	UPROPERTY()
	TObjectPtr<UPlayerOverlay> PlayerOverlay;
	
	UPROPERTY(EditAnywhere, Category="Widget")
	TSubclassOf<UWeaponWidget> WeaponOverlayClass;
	UPROPERTY()
	TObjectPtr<UWeaponWidget> WeaponOverlay;

	void CrouchingAct(bool IsTrigger);
	void SprintAct(bool IsTrigger);
	void JumpAct(bool IsTrigger);
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	TObjectPtr<UAmmoBoxComponent> AmmoBox;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	TObjectPtr<UUnrealPlayerInputComponent> InputConnector;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	TObjectPtr<UUnrealTabActionComponent> TabAction;

	virtual void OnPossess(APawn* InPawn) override;
	virtual void OnRep_Pawn() override;

protected:
	virtual void SetupInputComponent() override;
	virtual void BeginPlay() override;
	
public:
	AUnrealPlayerController();
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void Tick(float DeltaTime) override;
	
	virtual TObjectPtr<ABCharacter> GetTargetCharacter();
	
	virtual void TabButtonPressed();
	virtual void TabButtonReleased();
};
