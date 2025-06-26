
#include "InteractableItem.h"
#include "Prepper/Prepper.h"
#include "Components/BoxComponent.h"
#include "Components/WidgetComponent.h"
#include "Prepper/GamePlay/InteractionEventComponent/InteractionItemAddEventComponent.h"

AInteractableItem::AInteractableItem()
{
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;
	
	ItemMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ItemMesh"));
	SetRootComponent(ItemMesh);
	
	ItemMesh->SetCollisionResponseToAllChannels(ECR_Overlap);
	ItemMesh->SetCollisionResponseToChannel(ECC_Pawn, ECR_Ignore);
	ItemMesh->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
	ItemMesh->SetCollisionResponseToChannel(ECC_Vehicle, ECR_Ignore);
	ItemMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	ItemMesh->SetRenderCustomDepth(true);
	ItemMesh->SetCustomDepthStencilValue(CUSTOM_DEPTH_PURPLE);
	
	AreaBox = CreateDefaultSubobject<UBoxComponent>("AreaBox");
	AreaBox->SetupAttachment(RootComponent);
	AreaBox->SetCollisionResponseToAllChannels(ECR_Overlap);
	AreaBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	PickUpWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("PickUpWidget"));
	PickUpWidget->SetupAttachment(RootComponent);

	InteractionEvent = CreateDefaultSubobject<UInteractionItemAddEventComponent>(TEXT("AddItem"));
}

void AInteractableItem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (ItemMesh)
	{
		ItemMesh->AddWorldRotation(FRotator(0.f, BaseTurnRate * DeltaTime, 0.f));
	}
}

void AInteractableItem::BeginPlay()
{
	Super::BeginPlay();
	
	if(PickUpWidget)
	{
		PickUpWidget->SetVisibility(false);
	}
}

void AInteractableItem::Destroyed()
{
	Super::Destroyed();

}


