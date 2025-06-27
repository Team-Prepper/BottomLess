// Fill out your copyright notice in the Description page of Project Settings.


#include "DeathMatchInitialSettingComponent.h"

#include "Blueprint/UserWidget.h"
#include "Prepper/_DeathMatchMode/PlayerState/DeathMatchPlayerState.h"
#include "Prepper/Unreal/Controller/PlayerController/UnrealPlayerController.h"
#include "Prepper/_DeathMatchMode/UI/DeathMatchWidget.h"
#include "Prepper/___Legacy/HUD/UI/Announcement.h"

// Sets default values for this component's properties
UDeathMatchInitialSettingComponent::UDeathMatchInitialSettingComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	// ...
}

void UDeathMatchInitialSettingComponent::Initial(TObjectPtr<AUnrealPlayerController> Target)
{
	Super::Initial(Target);
	
	if (DeathMatchWidgetClass && DeathMatchWidget == nullptr)
	{
		DeathMatchWidget = CreateWidget<UDeathMatchWidget>(Target, DeathMatchWidgetClass);
		DeathMatchWidget->AddToViewport();
	}
	if (AnnounceWidgetClass && AnnounceWidget == nullptr)
	{
		AnnounceWidget = CreateWidget<UAnnouncement>(Target, AnnounceWidgetClass);
		AnnounceWidget->AddToViewport();
	}

	const TObjectPtr<ADeathMatchPlayerState> State = Target->GetPlayerState<ADeathMatchPlayerState>();
	
	if (State == nullptr) return;
	State->Attach(this);
	
}

void UDeathMatchInitialSettingComponent::Update(const ADeathMatchPlayerState& NewData)
{
	if (DeathMatchWidget == nullptr) return;
	
	DeathMatchWidget->SetScore(NewData.GetScore());
	DeathMatchWidget->SetDefeat(NewData.GetDefeat());
}

