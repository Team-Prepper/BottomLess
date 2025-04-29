// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
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
	UFUNCTION()
	void OnRep_Aiming();
	
	bool IsCrouchingLocal;
	bool IsAimingLocal;
	bool IsSprintLocal;

	bool IsLocal;

	void CrouchingAct(bool IsTrigger) const;
	void JumpAct(bool IsTrigger) const;

protected:
	virtual void BeginPlay() override;
public:
	// Sets default values for this component's properties
	UUnrealCharacterMoveComponent();

	float GetSpeed() const;
	
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
	void CrouchToggle();
	void JumpTrigger(bool IsTrigger);
	void SprintTrigger(bool IsTrigger);
	void SetAiming(bool IsTrigger);
	
	UFUNCTION(Server, Reliable)
	void ServerJumpTrigger(bool IsTrigger);
	UFUNCTION(Server, Reliable)
	void ServerSprintTrigger(bool IsTrigger);
	UFUNCTION(Server, Reliable)
	void ServerCrouchTrigger(bool IsTrigger);
	
};
