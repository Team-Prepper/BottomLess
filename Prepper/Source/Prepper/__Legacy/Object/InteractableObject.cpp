// Fill out your copyright notice in the Description page of Project Settings.


#include "InteractableObject.h"
#include "Prepper/Prepper.h"
#include "Components/BoxComponent.h"
#include "Components/WidgetComponent.h"

AInteractableObject::AInteractableObject()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	
	ItemMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("ObjectMesh"));
	SetRootComponent(ItemMesh);
	
	ItemMesh->SetCollisionResponseToAllChannels(ECR_Block);
	ItemMesh->SetCollisionResponseToChannel(ECC_Pawn, ECR_Ignore);
	ItemMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	ItemMesh->SetRenderCustomDepth(true);
	ItemMesh->SetCustomDepthStencilValue(CUSTOM_DEPTH_PURPLE);
	
	AreaBox = CreateDefaultSubobject<UBoxComponent>("AreaBox");
	AreaBox->SetupAttachment(RootComponent);
	AreaBox->SetCollisionResponseToAllChannels(ECR_Ignore);
	AreaBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	PickUpWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("InteractWidget"));
	PickUpWidget->SetupAttachment(RootComponent);
}

// Called when the game starts or when spawned
void AInteractableObject::BeginPlay()
{
	Super::BeginPlay();

	CustomDepthColor = CUSTOM_DEPTH_PURPLE;
}

// Called when the game starts or when spawned
void AInteractableObject::Interaction(APlayerCharacter* Target)
{
	InteractAction();
	UE_LOG(LogTemp, Warning, TEXT("Interaction Object : Interaction"));
}