// Fill out your copyright notice in the Description page of Project Settings.


#include "SensitivityChangeEffect.h"

#include "Prepper/__Legacy/Character/PlayerCharacter.h"

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
