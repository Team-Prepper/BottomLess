// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Prepper/GamePlay/Character/Component/CharacterMoveComponent.h"
#include "UnrealCharacterMoveComponent.generated.h"


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class PREPPER_API UUnrealCharacterMoveComponent : public UCharacterMoveComponent
{
	GENERATED_BODY()

	UPROPERTY(ReplicatedUsing=OnRep_Sprint)
	bool IsSprintNetwork;
	UPROPERTY(ReplicatedUsing=OnRep_Aiming)
	bool IsAimingNetwork;
	UPROPERTY(ReplicatedUsing=OnRep_Crouching)
	bool IsCrouchingNetwork;
	UPROPERTY(ReplicatedUsing=OnRep_Jump)
	bool IsJumpNetwork;
	UPROPERTY(ReplicatedUsing=OnRep_Car)
	ACar* TargetCarNetwork;

	UFUNCTION()
	void OnRep_Sprint();
	UFUNCTION()
	void OnRep_Aiming();
	UFUNCTION()
	void OnRep_Crouching();
	UFUNCTION()
	void OnRep_Jump();
	UFUNCTION()
	void OnRep_Car();

	bool IsLocal;

protected:
	virtual void BeginPlay() override;
public:
	// Sets default values for this component's properties
	UUnrealCharacterMoveComponent();
	
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
	virtual void CrouchToggle() override;
	virtual void JumpTrigger(bool IsTrigger) override;
	virtual void SprintTrigger(bool IsTrigger) override;
	virtual void SetAiming(bool IsTrigger) override;
	virtual void SetCar(TObjectPtr<ACar> Vehicle) override;
	
	UFUNCTION(Server, Reliable)
	void ServerSprintTrigger(bool IsTrigger);
	UFUNCTION(Server, Reliable)
	void ServerAimingTrigger(bool IsTrigger);
	UFUNCTION(Server, Reliable)
	void ServerJumpTrigger(bool IsTrigger);
	UFUNCTION(Server, Reliable)
	void ServerCrouchTrigger(bool IsTrigger);
	
};
