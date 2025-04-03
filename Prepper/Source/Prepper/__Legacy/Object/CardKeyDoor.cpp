// Fill out your copyright notice in the Description page of Project Settings.


#include "CardKeyDoor.h"

#include "Components/BoxComponent.h"
#include "Components/WidgetComponent.h"
#include "Prepper/__Legacy/Character/PlayerCharacter.h"
#include "Prepper/__Legacy/Item/Inventory/MapInventory.h"


// Sets default values
ACardKeyDoor::ACardKeyDoor()
{
	DoorFrame = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("DoorFrame"));
	SetRootComponent(DoorFrame);
	DoorInteract = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("DoorInteract"));
	DoorInteract->SetupAttachment(RootComponent);
	AreaBox = CreateDefaultSubobject<UBoxComponent>("AreaBox");
	AreaBox->SetupAttachment(RootComponent);
	AreaBox->SetCollisionResponseToAllChannels(ECR_Ignore);
	AreaBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	PickUpWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("PickUpWidget"));
	PickUpWidget->SetupAttachment(RootComponent);
	
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;
}

void ACardKeyDoor::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ACardKeyDoor, DoorInteract);
}

void ACardKeyDoor::Interaction(APlayerCharacter* Target)
{
	if (Target->GetInventory()->TryGetItemCount(TargetItemCode) < 1) return;

	if (IsClosed)
	{
		if (!Target->GetInventory()->TryUseItem(TargetItemCode, 1) ) return;
		Mul_OpenDoor();
		return;
	}
	
	CloseDoor();
}

void ACardKeyDoor::Mul_OpenDoor_Implementation()
{
	OpenDoor();
}

