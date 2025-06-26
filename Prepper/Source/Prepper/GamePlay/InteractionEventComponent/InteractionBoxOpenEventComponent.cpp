// Fill out your copyright notice in the Description page of Project Settings.


#include "InteractionBoxOpenEventComponent.h"


// Sets default values for this component's properties
UInteractionBoxOpenEventComponent::UInteractionBoxOpenEventComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	// ...
}

void UInteractionBoxOpenEventComponent::SetBoxMesh(const TObjectPtr<UStaticMeshComponent> TargetBoxMesh)
{
	BoxMesh = TargetBoxMesh;
}

void UInteractionBoxOpenEventComponent::Interaction(APlayerCharacter* Target)
{
	BoxOpen();
}

void UInteractionBoxOpenEventComponent::Interaction(ICharacterController* Target)
{
	BoxOpen();
}

void UInteractionBoxOpenEventComponent::BeginPlay()
{
	Super::BeginPlay();

	IsOpen = false;
}

void UInteractionBoxOpenEventComponent::BoxOpen()
{
	UE_LOG(LogTemp, Warning, TEXT("TryBoxOpen"));
	
	if(IsOpen) return;
	
	UE_LOG(LogTemp, Warning, TEXT("BoxOpen"));

	IsOpen = true;
	
	BoxOpenAct();

	if (SpawnedActorClasses.Num() < 1) return;

	const FActorSpawnParameters SpawnParams;
	const FVector SpawnLocation = GetOwner()->GetActorLocation() + FVector(0, 0, 50);
	FRotator SpawnRotation = GetOwner()->GetActorRotation();
	SpawnRotation.Yaw += 90.0f;

	const int RandNum = FMath::RandRange(0, SpawnedActorClasses.Num() - 1);
    
	// 블루프린트를 동적으로 생성
	GetWorld()->SpawnActor<AActor>(SpawnedActorClasses[RandNum], SpawnLocation, SpawnRotation, SpawnParams);
}

void UInteractionBoxOpenEventComponent::BoxOpenAct()
{
	UE_LOG(LogTemp,Warning, TEXT("MULTI OPEN"));
	
	BoxMesh->SetStaticMesh(OpenBoxMesh);
	BoxMesh->SetRenderCustomDepth(false);
	//ToggleTrigger(false);
	
	// Set a timer to destroy the box after 5 seconds
	GetWorld()->GetTimerManager().SetTimer(DestructionTimerHandle,
		this, &UInteractionBoxOpenEventComponent::DestroyBox, 5.0f, false);
}

void UInteractionBoxOpenEventComponent::DestroyBox() const
{
	GetOwner()->Destroy();
}
