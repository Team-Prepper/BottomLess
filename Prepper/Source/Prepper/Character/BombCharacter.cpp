﻿// Fill out your copyright notice in the Description page of Project Settings.


#include "BombCharacter.h"

#include "PlayerCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "Prepper/GameMode/PrepperGameMode.h"
#include "Prepper/Object/ElectricSwitch.h"


void ABombCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
}

// Sets default values
ABombCharacter::ABombCharacter()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void ABombCharacter::BeginPlay()
{
	Super::BeginPlay();
	TimeSend = 0;
	
}

// Called every frame
void ABombCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	TimeSend += DeltaTime;

	if (TimeSend < MaxTime) return;

	Bomb();
}

void ABombCharacter::Bomb()
{
	TArray<AActor*> Players;
	UGameplayStatics::GetAllActorsOfClass(this, APlayerCharacter::StaticClass(), Players);

	for (auto Player : Players)
	{
		Cast<APlayerCharacter>(Player)->ReceiveDamage(1000, nullptr, nullptr);
	}

	Destroy();
}

void ABombCharacter::SetElectricSwitch(TObjectPtr<AElectricSwitch> Switch)
{
	TargetSwitch = Switch;
}

void ABombCharacter::RemoveAction()
{
	Super::RemoveAction();
	if (TargetSwitch == nullptr) return;
	TargetSwitch->DefenderRemove();
}
