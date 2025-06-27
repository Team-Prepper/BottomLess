// Fill out your copyright notice in the Description page of Project Settings.


#include "InteractionCraftingEventComponent.h"

#include "Blueprint/UserWidget.h"
#include "Prepper/GamePlay/CharacterController/CharacterController.h"
#include "Prepper/GamePlay/Item/ItemCombination/UI/ItemCombinationUI.h"
#include "Prepper/___Legacy/Character/PlayerCharacter.h"


// Sets default values for this component's properties
UInteractionCraftingEventComponent::UInteractionCraftingEventComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
	
}

void UInteractionCraftingEventComponent::Interaction(APlayerCharacter* Target)
{
	InteractionAct(Target, Target->GetInventory());
}

void UInteractionCraftingEventComponent::Interaction(ICharacterController* Target)
{
	InteractionAct(Target->GetPawn(), Target->GetInventory());
}

void UInteractionCraftingEventComponent::TickComponent(float DeltaTime, ELevelTick TickType,
	FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	
	if (TargetPlayer == nullptr) return;
	if (FVector::DistSquared(TargetPlayer->GetActorLocation(), GetOwner()->GetActorLocation()) < Distance) return;
	
	TargetPlayer = nullptr;
	
	if (CombinationWidget == nullptr) return;

	UE_LOG(LogTemp, Warning, TEXT("Too Far"));
	CombinationWidget->SetVisibility(ESlateVisibility::Hidden);
	
}

void UInteractionCraftingEventComponent::InteractionAct(const TObjectPtr<APawn> Target, const TObjectPtr<UInventoryComponent> TargetInventory)
{
	if(!Target->IsLocallyControlled()) return;

	TargetPlayer = Target;
	APlayerController* PC = Target->GetController<APlayerController>();

	if (PC == nullptr) return;

	if (CombinationWidget == nullptr)
	{
		if (CombinationWidgetClass == nullptr) return;
		
		CombinationWidget = CreateWidget<UItemCombinationUI>(PC, CombinationWidgetClass);
		CombinationWidget->AddToViewport();
	}

	if (CombinationWidget == nullptr) return;

	CombinationWidget->SetTargetInventory(TargetInventory);
	CombinationWidget->SetVisibility(ESlateVisibility::Visible);
}
