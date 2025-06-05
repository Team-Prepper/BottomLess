// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Prepper/GamePlay/CharacterController/StatusComponent.h"
#include "UnrealCharacterMoveComponent.generated.h"


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class PREPPER_API UUnrealCharacterMoveComponent : public UActorComponent
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, Category = "Player Movement Speed")
	float WalkSpeed = 600;
	UPROPERTY(EditAnywhere, Category = "Player Movement Speed")
	float SprintSpeed = 900;
	UPROPERTY(EditAnywhere, Category = "Player Movement Speed")
	float AimMovementSpeed = 400.f;

	UPROPERTY(ReplicatedUsing=OnRep_Sprint)
	bool IsSprintNetwork;
	UPROPERTY(ReplicatedUsing=OnRep_Aiming)
	bool IsAimingNetwork;
	UPROPERTY(ReplicatedUsing=OnRep_Crouching)
	bool IsCrouchingNetwork;
	UPROPERTY(ReplicatedUsing=OnRep_Jump)
	bool IsJumpNetwork;

	UFUNCTION()
	void OnRep_Sprint();
	UFUNCTION()
	void OnRep_Aiming();
	UFUNCTION()
	void OnRep_Crouching();
	UFUNCTION()
	void OnRep_Jump();
	
	bool IsAimingLocal;
	bool IsSprintLocal;
	bool IsCrouchingLocal;

	bool IsLocal;

	void CrouchingAct(bool IsTrigger) const;
	void JumpAct(bool IsTrigger) const;
	void SetOwnerSpeed();

protected:
	virtual void BeginPlay() override;
public:
	// Sets default values for this component's properties
	UUnrealCharacterMoveComponent();

	float CoefficientMovementSpeed = 1;
	float GetSpeed() const;
	
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
	void CrouchToggle();
	void JumpTrigger(bool IsTrigger);
	void SprintTrigger(bool IsTrigger);
	void SetAiming(bool IsTrigger);

	bool IsSprint() const { return IsSprintLocal; }
	bool IsAiming() const { return IsAimingLocal; }
	
	UFUNCTION(Server, Reliable)
	void ServerSprintTrigger(bool IsTrigger);
	UFUNCTION(Server, Reliable)
	void ServerAimingTrigger(bool IsTrigger);
	UFUNCTION(Server, Reliable)
	void ServerJumpTrigger(bool IsTrigger);
	UFUNCTION(Server, Reliable)
	void ServerCrouchTrigger(bool IsTrigger);
	
};
