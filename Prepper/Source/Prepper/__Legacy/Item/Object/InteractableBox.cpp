// Fill out your copyright notice in the Description page of Project Settings.


#include "InteractableBox.h"

#include "Components/BoxComponent.h"
#include "Components/WidgetComponent.h"
#include "Prepper/Prepper.h"

AInteractableBox::AInteractableBox()
{
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;

	BoxMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BoxMesh"));
	SetRootComponent(BoxMesh);
	BoxMesh->SetCollisionResponseToAllChannels(ECR_Block);
	BoxMesh->SetRenderCustomDepth(true);
	BoxMesh->SetCustomDepthStencilValue(CustomDepthColor);

	AreaBox = CreateDefaultSubobject<UBoxComponent>("AreaBox");
	AreaBox->SetupAttachment(RootComponent);
	ToggleTrigger(false);
	
	PickUpWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("PickUpWidget"));
	PickUpWidget->SetupAttachment(RootComponent);
}

void AInteractableBox::BeginPlay()	
{
	Super::BeginPlay();

	CustomDepthColor = CUSTOM_DEPTH_MINT;

	BoxMesh->SetCustomDepthStencilValue(CustomDepthColor);
	BoxMesh->MarkRenderStateDirty();

	IsOpen = false;
}

void AInteractableBox::Interaction(APlayerCharacter* Target)
{
	// interaction은 서버만 호출됨
	
	if(IsOpen) return;
	
	IsOpen = true;
	
	MulticastBoxOpen();

	if (SpawnedActorClasses.Num() > 0)
	{
		FActorSpawnParameters SpawnParams;
		FVector SpawnLocation = GetActorLocation() + FVector(0, 0, 50);
		FRotator SpawnRotation = GetActorRotation();
		SpawnRotation.Yaw += 90.0f;
    
		// 블루프린트를 동적으로 생성
		GetWorld()->SpawnActor<AActor>(SpawnedActorClasses[FMath::RandRange(0, SpawnedActorClasses.Num() - 1)], SpawnLocation, SpawnRotation, SpawnParams);
	}
}

void AInteractableBox::MulticastBoxOpen_Implementation()
{
	IsOpen = true;
	UE_LOG(LogTemp,Warning, TEXT("MULTI OPEN"));
	BoxMesh->SetStaticMesh(OpenBoxMesh);
	BoxMesh->SetRenderCustomDepth(false);
	ToggleTrigger(false);
	// Set a timer to destroy the box after 5 seconds
	GetWorld()->GetTimerManager().SetTimer(DestructionTimerHandle, this, &AInteractableBox::MulticastDestroyBox, 5.0f, false);
}

void AInteractableBox::MulticastDestroyBox_Implementation()
{
	Destroy();
}