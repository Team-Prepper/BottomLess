
#include "InteractableActor.h"
#include "Components/BoxComponent.h"
#include "Components/WidgetComponent.h"
#include "Prepper/Prepper.h"
#include "Prepper/GamePlay/InteractionEventComponent/InteractionEventComponent.h"

AInteractableActor::AInteractableActor()
{
	ColliderChannel.SetAllChannels(ECR_Block);
	ColliderChannel.SetResponse(ECC_Pawn, ECR_Ignore);
	ColliderChannel.SetResponse(ECC_Camera, ECR_Ignore);
	ColliderChannel.SetResponse(ECC_Vehicle, ECR_Ignore);
	
	TriggerChannel.SetAllChannels(ECR_Overlap);
	TriggerChannel.SetResponse(ECC_Pawn, ECR_Ignore);
	TriggerChannel.SetResponse(ECC_Camera, ECR_Ignore);

}

void AInteractableActor::BeginPlay()
{
	Super::BeginPlay();

	if(AreaBox)
	{
		AreaBox->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		AreaBox->SetCollisionObjectType(ECC_InteractMesh);
		AreaBox->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	}
	
	if(PickUpWidget)
	{
		PickUpWidget->SetVisibility(false);
	}
}

void AInteractableActor::ShowPickUpWidget(bool bShowWidget)
{
	if(PickUpWidget)
	{
		PickUpWidget->SetVisibility(bShowWidget);
	}
}

void AInteractableActor::ToggleTrigger(const bool bEnable)
{
	if (!bEnable)
	{
		AreaBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		return;
	}
	
	AreaBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	AreaBox->SetCollisionResponseToChannels(TriggerChannel);
}

void AInteractableActor::Interaction(APlayerCharacter* Target)
{
	if (InteractionEvent == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("EventNull"));
		return;
	}
	
	InteractionEvent->Interaction(Target);
}

void AInteractableActor::Interaction(ICharacterController* Target)
{
	if (InteractionEvent == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("EventNull"));
		return;
	}
	InteractionEvent->Interaction(Target);
	
}
