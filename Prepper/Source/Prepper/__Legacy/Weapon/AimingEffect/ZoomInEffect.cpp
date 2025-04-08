// Fill out your copyright notice in the Description page of Project Settings.


#include "ZoomInEffect.h"

#include "Prepper/__Legacy/Character/PlayerCharacter.h"
#include "Prepper/GamePlay/Character/BCharacter.h"

void UZoomInEffect::PlayerAimingStart(const TObjectPtr<APlayerCharacter> PlayerCharacter)
{
	TargetPlayer = PlayerCharacter;
	TargetPlayer->GetFollowCamera()->InterpFOV(GoalFOV, ZoomSpeed);
}

void UZoomInEffect::PlayerAimingEnd()
{
	if (TargetPlayer == nullptr) return;
	TargetPlayer->GetFollowCamera()->InterpFOV(TargetPlayer->GetFollowCamera()->DefaultFOV, ZoomSpeed);
	
}

void UZoomInEffect::CharacterAimingStart(const TObjectPtr<ABCharacter> Character)
{
	TargetCharacter = Character;
	//TargetCharacter->GetFollowCamera()->InterpFOV(GoalFOV, ZoomSpeed);
}

void UZoomInEffect::CharacterAimingEnd()
{
	if (TargetCharacter == nullptr) return;
	//TargetCharacter->GetFollowCamera()->InterpFOV(TargetCharacter->GetFollowCamera()->DefaultFOV, ZoomSpeed);
}
