// Fill out your copyright notice in the Description page of Project Settings.


#include "InteractionComponent.h"

#include "Kismet/GameplayStatics.h"
#include "Prepper/Prepper.h"
#include "Prepper/__Legacy/Character/BaseCharacter.h"
#include "Prepper/__Legacy/Interfaces/Interactable.h"

UInteractionComponent::UInteractionComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

	Character = nullptr;
}

void UInteractionComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	FHitResult HitResult;
	TraceInteractionItem(HitResult);
}


void UInteractionComponent::SetCharacter(ABaseCharacter* Target)
{
	Character = Target;
}

void UInteractionComponent::TraceInteractionItem(FHitResult& TraceHitResult)
{
	FVector2D ViewportSize;
	if (GEngine && GEngine->GameViewport)
	{
		GEngine->GameViewport->GetViewportSize(ViewportSize);
	}

	const FVector2D CrosshairLocation(ViewportSize.X / 2.f, ViewportSize.Y / 2.f);
	FVector CrosshairWorldPosition;
	FVector CrosshairWorldDirection;
	const bool bScreenToWorld = UGameplayStatics::DeprojectScreenToWorld(
		UGameplayStatics::GetPlayerController(this, 0),
		CrosshairLocation,
		CrosshairWorldPosition,
		CrosshairWorldDirection
	);

	if (!bScreenToWorld) return;
	
	FVector Start = CrosshairWorldPosition;
	FVector End = Start + CrosshairWorldDirection * TraceRange;

	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(Character);
		
	bool bHitSomething = GetWorld()->LineTraceSingleByObjectType(
		TraceHitResult,
		Start,
		End,
		FCollisionObjectQueryParams(ECC_InteractMesh), // 동적 물체와 충돌 검사
		QueryParams
	);

	if (bHitSomething)
	{
		SetItemInteractable(TraceHitResult.GetActor());
	}
	else
	{
		SetItemInteractable(nullptr);
	}
}

void UInteractionComponent::SetItemInteractable(AActor* InteractableItem)
{
	if(CurInteractableItem != nullptr && InteractableItem == nullptr)
	{
		CurInteractableItem->ShowPickUpWidget(false);
		if(!Character->IsLocallyControlled()) return;
		
		CurInteractableItem = nullptr;
		
		if(!Character->HasAuthority())
		{
			UE_LOG(LogTemp, Warning, TEXT("Set Null Interaction"));
			ServerSetItemInteractable(nullptr);
		}
		return;
	}
	
	TScriptInterface<IInteractable> NewInteractableItem = TScriptInterface<IInteractable>(InteractableItem);
	
	if (NewInteractableItem == CurInteractableItem) return;
	if (!Character->IsLocallyControlled()) return;
	if(!Character->HasAuthority())
	{
		ServerSetItemInteractable(InteractableItem);
	}
	if(CurInteractableItem)
	{
		CurInteractableItem->ShowPickUpWidget(false);
	}
	CurInteractableItem = NewInteractableItem;
	if(CurInteractableItem)
	{
		UE_LOG(LogTemp, Warning, TEXT("Set Interaction Item"));
		CurInteractableItem->ShowPickUpWidget(true);
	}
}

void UInteractionComponent::ServerSetItemInteractable_Implementation(AActor* InteractableItem)
{
	if(InteractableItem == nullptr)
	{
		CurInteractableItem = nullptr;
		return;
	}
	CurInteractableItem = TScriptInterface<IInteractable>(InteractableItem);
}
