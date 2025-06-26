// Fill out your copyright notice in the Description page of Project Settings.


#include "InteractionExitEventComponent.h"

#include "Prepper/GamePlay/CharacterController/CharacterController.h"
#include "Prepper/_SurvivorMode/SurvivorGameMode.h"
#include "Prepper/___Legacy/PlayerController/SurvivorController.h"


// Sets default values for this component's properties
UInteractionExitEventComponent::UInteractionExitEventComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	// ...
}

void UInteractionExitEventComponent::Interaction(ICharacterController* Target)
{
	Super::Interaction(Target);
	ASurvivorGameMode* GM = GetWorld()->GetAuthGameMode<ASurvivorGameMode>();
	
	if (GM == nullptr) return;
	if (!GM->IsAchieved(TargetAchievement)) return;

	TObjectPtr<ASurvivorController> PC = Target->GetPawn()->GetController<ASurvivorController>();
	if (!PC) return;

	PC->MulticastShowGameEnd();
}

