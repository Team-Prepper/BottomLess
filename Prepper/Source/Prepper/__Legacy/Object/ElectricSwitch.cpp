// Fill out your copyright notice in the Description page of Project Settings.


#include "ElectricSwitch.h"

#include "Prepper/GamePlay/GameMode/SurvivorGameMode.h"
#include "Prepper/__Legacy/Character/BombCharacter.h"

void AElectricSwitch::Interaction(APlayerCharacter* Target)
{
	Super::Interaction(Target);
	ASurvivorGameMode* GM = GetWorld()->GetAuthGameMode<ASurvivorGameMode>();
	if (GM == nullptr) return;

	if (GM->IsAchieved(TargetAchievement)) return;
	if (IsSpawned) return;
	
	CreateMonster();
	
}

void AElectricSwitch::DefenderWin()
{
	IsSpawned = false;
}

void AElectricSwitch::CreateMonster()
{
	IsSpawned = true;
	FRotator SpawnRotation = FRotator::ZeroRotator;
	GetWorld()->SpawnActor<ABombCharacter>(TargetMonster, SpawnPosition, SpawnRotation)->SetElectricSwitch(this);
}

void AElectricSwitch::DefenderRemove()
{
	ASurvivorGameMode* GM = GetWorld()->GetAuthGameMode<ASurvivorGameMode>();
	
	if (GM == nullptr) return;
	GM->AddAchievement(TargetAchievement, true);
}
