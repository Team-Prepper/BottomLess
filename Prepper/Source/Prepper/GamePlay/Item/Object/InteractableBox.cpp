// Fill out your copyright notice in the Description page of Project Settings.


#include "InteractableBox.h"

#include "Components/BoxComponent.h"
#include "Components/WidgetComponent.h"
#include "Prepper/Prepper.h"
#include "Prepper/GamePlay/InteractionEventComponent/InteractionBoxOpenEventComponent.h"

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

	const TObjectPtr<UInteractionBoxOpenEventComponent> BoxOpen
		= CreateDefaultSubobject<UInteractionBoxOpenEventComponent>(TEXT("BoxOpenEvent"));
	BoxOpen->SetBoxMesh(BoxMesh);

	InteractionEvent = BoxOpen;
}

void AInteractableBox::BeginPlay()	
{
	Super::BeginPlay();

	CustomDepthColor = CUSTOM_DEPTH_MINT;

	BoxMesh->SetCustomDepthStencilValue(CustomDepthColor);
	BoxMesh->MarkRenderStateDirty();
	
}