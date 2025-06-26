// Fill out your copyright notice in the Description page of Project Settings.


#include "EscapeActionComponent.h"

#include "Blueprint/UserWidget.h"
#include "Kismet/GameplayStatics.h"


// Sets default values for this component's properties
UEscapeActionComponent::UEscapeActionComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	// ...
}

void UEscapeActionComponent::EscapePressed()
{
	if (SettingWidgetClass != nullptr && SettingWidget == nullptr)
	{
		SettingWidget = CreateWidget<UUserWidget>(GetOwner<APlayerController>(), SettingWidgetClass);
		//SettingWidget->SetVisibility(ESlateVisibility::Hidden);
	}
	if (SettingWidget == nullptr) return;
	
	APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	if (PC == nullptr) return;
	
	if (SettingWidget->GetVisibility() == ESlateVisibility::Visible)
	{
		PC->SetInputMode(FInputModeGameOnly());
		PC->SetShowMouseCursor(false);
		SettingWidget->SetVisibility(ESlateVisibility::Hidden);
		SettingWidget->RemoveFromParent();
		return;
	}
	
	PC->SetInputMode(FInputModeGameAndUI());
	PC->SetShowMouseCursor(true);
	SettingWidget->AddToViewport();
	SettingWidget->SetVisibility(ESlateVisibility::Visible);
}
