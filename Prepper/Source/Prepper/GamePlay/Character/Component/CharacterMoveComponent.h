// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "CharacterMoveComponent.generated.h"


class ACar;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class PREPPER_API UCharacterMoveComponent : public UActorComponent
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, Category = "Player Movement Speed")
	float WalkSpeed = 600;
	UPROPERTY(EditAnywhere, Category = "Player Movement Speed")
	float SprintSpeed = 900;
	UPROPERTY(EditAnywhere, Category = "Player Movement Speed")
	float AimMovementSpeed = 400.f;

protected:
	bool IsAimingLocal;
	bool IsSprintLocal;
	bool IsCrouchingLocal;

	UPROPERTY()
	TObjectPtr<ACar> TargetCar;

	void CrouchingAct(bool IsTrigger) const;
	void JumpAct(bool IsTrigger) const;
	void SetOwnerSpeed() const;
	
public:
	// Sets default values for this component's properties
	UCharacterMoveComponent();
	float GetSpeed() const;
	float CoefficientMovementSpeed = 1;
	
	// Called every frame
	virtual void CrouchToggle();
	virtual void JumpTrigger(bool IsTrigger);
	virtual void SprintTrigger(bool IsTrigger);
	virtual void SetAiming(bool IsTrigger);

	virtual void SetCar(TObjectPtr<ACar> Vehicle);

	bool IsSprint() const { return IsSprintLocal; }
	bool IsAiming() const { return IsAimingLocal; }
	TObjectPtr<ACar> GetTargetCar() const { return TargetCar; }
};
