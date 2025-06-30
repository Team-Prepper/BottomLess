// Fill out your copyright notice in the Description page of Project Settings.


#include "SurvivorGameOverUI.h"

#include "Components/Button.h"
#include "Prepper/GamePlay/PrepperGameMode.h"
#include "Prepper/Unreal/Controller/PlayerController/UnrealPlayerController.h"
#include "Prepper/___Legacy/PlayerController/SurvivorController.h"

void USurvivorGameOverUI::NativeOnInitialized()
{
	Super::NativeOnInitialized();
	RetryButton->OnClicked.AddDynamic(this, &USurvivorGameOverUI::Retry);
}

void USurvivorGameOverUI::Retry()
{
	const TObjectPtr<ASurvivorController> PC = GetWorld()->GetFirstPlayerController<ASurvivorController>();
	
	if (PC != nullptr)
	{
		PC->ServerRespawnRequest(nullptr, PC);
		return;
	}

	const TObjectPtr<AUnrealPlayerController> UPC = GetWorld()->GetFirstPlayerController<AUnrealPlayerController>();

	UPC->Respawn();
}

void USurvivorGameOverUI::ServerRespawnRequest_Implementation(ACharacter* ElimmedCharacter, AController* ElimmedController)
{
	const TObjectPtr<APrepperGameMode> GM = GetWorld()->GetAuthGameMode<APrepperGameMode>();
	if (GM == nullptr) return;
	GM->RequestRespawn(ElimmedCharacter, ElimmedController);
}
