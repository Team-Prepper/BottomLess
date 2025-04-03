#include "CraftingTable.h"

#include "Prepper/__Legacy/Character/PlayerCharacter.h"
#include "Components/BoxComponent.h"
#include "Components/WidgetComponent.h"
#include "Prepper/__Legacy/HUD/UI/ItemCombination/ItemCombinationUI.h"
#include "Prepper/__Legacy/PlayerController/BasePlayerController.h"

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
}

void ACraftingTable::Interaction(APlayerCharacter* Target)
{
	MulticastInteraction(Target);
}

void ACraftingTable::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (TargetPlayer == nullptr) return;
	
	if (FVector::DistSquared(TargetPlayer->GetActorLocation(), GetActorLocation()) < Distance) return;

	TargetPlayer = nullptr;
	
	if (CombinationWidget == nullptr) return;
	CombinationWidget->SetVisibility(ESlateVisibility::Hidden);
}

void ACraftingTable::MulticastInteraction_Implementation(APlayerCharacter* Target)
{
	if(!Target->IsLocallyControlled()) return;

	TargetPlayer = Target;
	ABasePlayerController* PC = Target->GetController<ABasePlayerController>();

	if (PC == nullptr) return;

	if (CombinationWidget == nullptr)
	{
		if (CombinationWidgetClass == nullptr) return;
		
		CombinationWidget = CreateWidget<UItemCombinationUI>(PC, CombinationWidgetClass);
		CombinationWidget->AddToViewport();
	}

	if (CombinationWidget == nullptr) return;

	CombinationWidget->SetTargetPlayer(TargetPlayer);
	CombinationWidget->SetVisibility(ESlateVisibility::Visible);
	
	// DO SOMETHING
}

