// Fill out your copyright notice in the Description page of Project Settings.


#include "Equipment.h"

#include "Components/BoxComponent.h"
#include "Components/WidgetComponent.h"
#include "Prepper/Prepper.h"

void AEquipment::BeginPlay()
{
	Super::BeginPlay();
	
	if(InteractionArea)
	{
		InteractionArea->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		InteractionArea->SetCollisionObjectType(ECC_InteractMesh);
		InteractionArea->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	}
	
	if(InteractionWidget)
	{
		InteractionWidget->SetVisibility(false);
	}
}

void AEquipment::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	GetComponents<TObjectPtr<UMeshComponent>>(MeshComponents);
}

void AEquipment::ShowPickUpWidget(const bool bShowWidget)
{
	if(InteractionWidget == nullptr) return;
	InteractionWidget->SetVisibility(bShowWidget);
}

FString AEquipment::GetCode()
{
	return FString();
}

void AEquipment::ToggleOutline(const bool bEnable)
{
	for (const TObjectPtr<UMeshComponent> Mesh : MeshComponents)
	{
		Mesh->SetRenderCustomDepth(bEnable);
	}
}

void AEquipment::ToggleTrigger(const bool bEnable)
{
	if (!bEnable)
	{
		InteractionArea->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		return;
	}
	
	InteractionArea->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	InteractionArea->SetCollisionResponseToChannels(TriggerChannel);
}