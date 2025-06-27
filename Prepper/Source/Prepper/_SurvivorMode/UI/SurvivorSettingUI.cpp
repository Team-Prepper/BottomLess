// Fill out your copyright notice in the Description page of Project Settings.


#include "SurvivorSettingUI.h"

#include "Components/Button.h"
#include "Prepper/Unreal/Controller/PlayerController/UnrealPlayerController.h"
#include "Prepper/___Legacy/PlayerController/SurvivorController.h"


void USurvivorSettingUI::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	SaveButton->OnClicked.
		AddDynamic(this, &USurvivorSettingUI::LegacyDataSave);
	SaveButton->OnClicked.
			AddDynamic(this, &USurvivorSettingUI::DataSave);
}

void USurvivorSettingUI::LegacyDataSave()
{
	const TObjectPtr<ASurvivorController> PC = GetWorld()->GetFirstPlayerController<ASurvivorController>();
	if (PC == nullptr) return;

	PC->SaveGame();
}

void USurvivorSettingUI::DataSave()
{
	const TObjectPtr<AUnrealPlayerController> PC = GetWorld()->GetFirstPlayerController<AUnrealPlayerController>();
	if (PC == nullptr) return;

	PC->GameSaveAction();
}