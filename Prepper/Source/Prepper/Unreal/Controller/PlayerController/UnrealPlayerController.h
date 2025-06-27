// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "Prepper/GamePlay/CharacterController/CharacterController.h"
#include "UnrealPlayerController.generated.h"

class UEscapeActionComponent;
class UInitialSettingComponent;
class UWeaponWidget;
class UTabActionComponent;
class UUnrealCombatComponent;
class UUnrealStatusComponent;
class UUnrealPlayerInputComponent;
class UInteractionHandlingComponent;
class IControllerMapper;
class ABCharacter;
class ACar;
/**
 * 
 */

class UPlayerOverlay;

UCLASS()
class PREPPER_API AUnrealPlayerController : public APlayerController
{
	GENERATED_BODY()
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	TObjectPtr<UAmmoBoxComponent> AmmoBox;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	TObjectPtr<UUnrealPlayerInputComponent> InputConnector;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	TObjectPtr<UTabActionComponent> TabAction;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	TObjectPtr<UInitialSettingComponent> InitialSetting;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	TObjectPtr<UEscapeActionComponent> EscapeAction;

	UPROPERTY()
	TObjectPtr<ACar> TargetCar;
	UPROPERTY()
	TObjectPtr<ABCharacter> TargetCharacter;
	
	virtual void OnPossess(APawn* InPawn) override;
	virtual void OnRep_Pawn() override;

protected:
	virtual void SetupInputComponent() override;
	virtual void BeginPlay() override;
	
public:
	AUnrealPlayerController();
	virtual void Tick(float DeltaTime) override;
	
	virtual TObjectPtr<ABCharacter> GetTargetCharacter();
	virtual TObjectPtr<ACar> GetTargetCar();
	
	TObjectPtr<UAmmoBoxComponent> GetAmmoBox() const { return AmmoBox; }

	void TabButtonPressed() const;
	void TabButtonReleased() const;
	
	void EscapeButtonPressed() const;
};
