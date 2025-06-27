// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Prepper/GamePlay/CharacterController/GameSaveComponent.h"
#include "SurvivorGameSaveComponent.generated.h"


class ABCharacter;
class UAmmoBoxComponent;
class UInventoryComponent;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class PREPPER_API USurvivorGameSaveComponent : public UGameSaveComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	USurvivorGameSaveComponent();
	virtual void GameSave() override;
private:
	void SaveClientData();
	void SaveServerData();
	
};
