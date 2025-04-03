// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "Prepper/GamePlay/CharacterController/CharacterController.h"
#include "UnrealPlayerController.generated.h"

class UUnrealCombatComponent;
class UUnrealStatusComponent;
class UPlayerInputComponent;
class UInputAction;
class IControllerMapper;
class ABCharacter;

/**
 * 
 */
UCLASS()
class PREPPER_API AUnrealPlayerController :
		public APlayerController, public ICharacterController
{
	GENERATED_BODY()

	TObjectPtr<ABCharacter> TargetCharacter;

	UPROPERTY(ReplicatedUsing=OnRep_Crouching)
	bool IsCrouching;
	UPROPERTY(ReplicatedUsing=OnRep_Sprint)
	bool IsSprint;
	UPROPERTY(ReplicatedUsing=OnRep_Jump)
	bool IsJump;
	
	UFUNCTION()
	void OnRep_Crouching();
	UFUNCTION()
	void OnRep_Sprint();
	UFUNCTION()
	void OnRep_Jump();

	void CrouchingAct(bool IsTrigger);
	void SprintAct(bool IsTrigger);
	void JumpAct(bool IsTrigger);
	
	bool IsCrouchingLocal;
	bool IsSprintLocal;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	TObjectPtr<UPlayerInputComponent> InputConnector;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	TObjectPtr<UUnrealStatusComponent> Status;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	TObjectPtr<UUnrealCombatComponent> Combat;

protected:
	virtual void SetupInputComponent() override;
	virtual void BeginPlay() override;
	
public:
	AUnrealPlayerController();
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
	virtual TObjectPtr<ABCharacter> GetTargetCharacter() override;
	virtual IStatus* GetStatus() override;
	virtual ICombat* GetCombat() override;
	
	void PlayAnim(const FString& String);
	
	virtual void Move(const FInputActionValue& Value) override;
	virtual void Look(const FInputActionValue& Value) override;
	
	virtual void CrouchToggle() override;
	
	virtual void JumpTrigger(bool IsTrigger) override;
	virtual void SprintTrigger(bool IsTrigger) override;
	
	virtual void EquipButtonPressed() override;
	
	UFUNCTION(Server, Reliable)
	void ServerJumpTrigger(bool IsTrigger);
	UFUNCTION(Server, Reliable)
	void ServerSprintTrigger(bool IsTrigger);
	UFUNCTION(Server, Reliable)
	void ServerCrouchTrigger(bool IsTrigger);
	
	UFUNCTION(Server, Reliable)
	void ServerEquipButtonPressed();
};
