// Fill out your copyright notice in the Description page of Project Settings.


#include "ExitDoor.h"

#include "Prepper/_SurvivorMode/SurvivorGameMode.h"
#include "Prepper/___Legacy/Character/PlayerCharacter.h"
#include "Prepper/___Legacy/PlayerController/SurvivorController.h"


// Sets default values
AExitDoor::AExitDoor()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

void AExitDoor::Interaction(APlayerCharacter* Target)
{
	Super::Interaction(Target);
	ASurvivorGameMode* GM = GetWorld()->GetAuthGameMode<ASurvivorGameMode>();
	
	if (GM == nullptr) return;
	if (!GM->IsAchieved(TargetAchievement)) return;

	TObjectPtr<ASurvivorController> PC = Target->GetController<ASurvivorController>();
	if (!PC) return;

	PC->MulticastShowGameEnd();
}

// Called when the game starts or when spawned
void AExitDoor::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AExitDoor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

