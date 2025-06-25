// Fill out your copyright notice in the Description page of Project Settings.


#include "UnrealInteractionComponent.h"

#include "Prepper/Prepper.h"
#include "Prepper/GamePlay/Interactable.h"
#include "Prepper/GamePlay/Character/BCharacter.h"
#include "Prepper/GamePlay/Character/Component/BCombatComponent.h"


// Sets default values for this component's properties
UUnrealInteractionComponent::UUnrealInteractionComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	TargetCharacter = nullptr;
}

void UUnrealInteractionComponent::SetTargetCharacter(TObjectPtr<ABCharacter> Character)
{
	TargetCharacter = Character;
}


// Called when the game starts
void UUnrealInteractionComponent::BeginPlay()
{
	Super::BeginPlay();
}


// Called every frame
void UUnrealInteractionComponent::TickComponent(float DeltaTime, ELevelTick TickType,
                                                FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	FHitResult HitResult;
	TraceInteractionItem(HitResult);
	
}

void UUnrealInteractionComponent::TraceInteractionItem(FHitResult& TraceHitResult)
{
	if (TargetCharacter == nullptr) return;
	
	FVector Start;
	FVector Direction;
	
	TargetCharacter->GetLookDirection(Start, Direction);
	const FVector End = Start + Direction * TraceRange;
	
	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(TargetCharacter);

	if (GetWorld()->LineTraceSingleByObjectType(
		TraceHitResult,
		Start,
		End,
		FCollisionObjectQueryParams(ECC_InteractMesh), // 동적 물체와 충돌 검사
		QueryParams
	))
	{
		SetItemInteractable(TraceHitResult.GetActor());
		return;
	}
	
	SetItemInteractable(nullptr);
}


void UUnrealInteractionComponent::SetItemInteractable(const TScriptInterface<IInteractable> InteractableItem)
{
	if(CurInteractableItem != nullptr && InteractableItem == nullptr)
	{
		CurInteractableItem->ShowPickUpWidget(false);
		CurInteractableItem = nullptr;
		return;
	}
	
	if (InteractableItem == CurInteractableItem) return;
	
	if(CurInteractableItem)
	{
		CurInteractableItem->ShowPickUpWidget(false);
	}
	CurInteractableItem = InteractableItem;
	if(!CurInteractableItem) return;
	{
		CurInteractableItem->ShowPickUpWidget(true);
	}
}

void UUnrealInteractionComponent::Interaction()
{
	const TScriptInterface<IInteractable> Target = GetInteractable();
	
	if (Target == nullptr)
	{
		TargetCharacter->GetCombat()->Swap();
		return;
	}
	
	Target->Interaction(TargetCharacter);
}