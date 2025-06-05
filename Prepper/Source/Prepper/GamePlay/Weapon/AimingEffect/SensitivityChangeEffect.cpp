// Fill out your copyright notice in the Description page of Project Settings.


#include "SensitivityChangeEffect.h"

#include "Prepper/___Legacy/Character/PlayerCharacter.h"
#include "Prepper/GamePlay/Character/BCharacter.h"

void USensitivityChangeEffect::PlayerAimingStart(const TObjectPtr<APlayerCharacter> PlayerCharacter)
{
	TargetPlayer = PlayerCharacter;
	TargetPlayer->SetSensitivity(Effect);
}

void USensitivityChangeEffect::PlayerAimingEnd()
{
	if (TargetPlayer == nullptr) return;
	TargetPlayer->SetSensitivity(1);
}

void USensitivityChangeEffect::CharacterAimingStart(const TObjectPtr<ABCharacter> Character)
{
	
}

void USensitivityChangeEffect::CharacterAimingEnd()
{
	
}