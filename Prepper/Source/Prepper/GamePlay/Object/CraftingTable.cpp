#include "CraftingTable.h"

#include "Components/BoxComponent.h"
#include "Components/WidgetComponent.h"
#include "Prepper/GamePlay/InteractionEventComponent/InteractionCraftingEventComponent.h"

ACraftingTable::ACraftingTable()
{
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;

	TableMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BoxMesh"));
	SetRootComponent(TableMesh);
	TableMesh->SetCollisionResponseToAllChannels(ECR_Block);
	TableMesh->SetRenderCustomDepth(true);
	TableMesh->SetCustomDepthStencilValue(CustomDepthColor);

	AreaBox = CreateDefaultSubobject<UBoxComponent>("AreaBox");
	AreaBox->SetupAttachment(RootComponent);
	AreaBox->SetCollisionResponseToAllChannels(ECR_Ignore);
	AreaBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	
	PickUpWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("PickUpWidget"));
	PickUpWidget->SetupAttachment(RootComponent);

	InteractionEvent = CreateDefaultSubobject<UInteractionCraftingEventComponent>(TEXT("CraftingTable"));
}