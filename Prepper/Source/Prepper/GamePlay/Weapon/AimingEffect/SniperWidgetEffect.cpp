// Fill out your copyright notice in the Description page of Project Settings.


#include "SniperWidgetEffect.h"

#include "Blueprint/UserWidget.h"
#include "Prepper/GamePlay/Character/BCharacter.h"
#include "Prepper/___Legacy/Character/PlayerCharacter.h"

void USniperWidgetEffect::PlayerAimingStart(TObjectPtr<APlayerCharacter> Player)
{
	if (SniperWidget == nullptr)
	{
		TObjectPtr<APlayerController> PC = Cast<APlayerController>(Player->GetController());
		SniperWidget = CreateWidget(PC, SniperWidgetClass);
		SniperWidget->AddToViewport();
	}
	if (SniperWidget == nullptr) return;
	
	SniperWidget->SetVisibility(ESlateVisibility::Visible);
}

void USniperWidgetEffect::PlayerAimingEnd()
{
	if (SniperWidget == nullptr) return;
	
	SniperWidget->SetVisibility(ESlateVisibility::Hidden);
}

void USniperWidgetEffect::CharacterAimingStart(const TObjectPtr<ABCharacter> Character)
{
	if (SniperWidget == nullptr)
	{
		TObjectPtr<APlayerController> PC = Cast<APlayerController>(Character->GetController());
		SniperWidget = CreateWidget(PC, SniperWidgetClass);
		SniperWidget->AddToViewport();
	}
	if (SniperWidget == nullptr) return;
	
	SniperWidget->SetVisibility(ESlateVisibility::Visible);
}

void USniperWidgetEffect::CharacterAimingEnd()
{
	if (SniperWidget == nullptr) return;
	
	SniperWidget->SetVisibility(ESlateVisibility::Hidden);
}
