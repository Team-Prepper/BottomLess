// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InteractableObject.h"
#include "ElectricSwitch.generated.h"

class ABombCharacter;
/**
 * 
 */
UCLASS()
class PREPPER_API AElectricSwitch : public AInteractableObject
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere)
	FString TargetAchievement;
	UPROPERTY(EditAnywhere)
	TSubclassOf<ABombCharacter> TargetMonster;
	bool IsSpawned = false;
	
public:
	virtual void Interaction(APlayerCharacter* Target) override;
	void DefenderWin();
	void CreateMonster();
	void DefenderRemove();
private:
	UPROPERTY(EditAnywhere, Category = "SpawnPosition",meta = (AllowPrivateAccess = "true"))
	FVector SpawnPosition;
};
