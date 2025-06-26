// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InteractionEventComponent.h"
#include "InteractionAddAmmoEventComponent.generated.h"


class IAmmoBox;
enum class EWeaponType : uint8;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class PREPPER_API UInteractionAddAmmoEventComponent : public UInteractionEventComponent
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	int32 AmmoAmount = 30;

	UPROPERTY(EditAnywhere)
	EWeaponType WeaponType;
	
public:
	// Sets default values for this component's properties
	UInteractionAddAmmoEventComponent();

	virtual void Interaction(APlayerCharacter *Target) override;
	virtual void Interaction(ICharacterController* Target) override;

private:
	void AddAmmo(IAmmoBox* AmmoBox);
};
