// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "PrepperGameMode.generated.h"

class APlayerCharacter;
class ABaseCharacter;
class ABasePlayerController;

/**
 * 
 */
UCLASS()
class PREPPER_API APrepperGameMode : public AGameMode
{
	GENERATED_BODY()
public:
	
	virtual void PlayerEliminated(ABaseCharacter* ElimmedCharacter);
	virtual void PlayerEliminated(ABaseCharacter* ElimmedCharacter,
									ABasePlayerController* VictimController,
									ABasePlayerController* AttackerController);
	
	virtual void RequestRespawn(ACharacter* ElimmedCharacter, AController* ElimmedController);
};
