// Fill out your copyright notice in the Description page of Project Settings.


#include "InitialSettingComponent.h"

#include "Blueprint/UserWidget.h"
#include "Prepper/GamePlay/Character/BCharacter.h"
#include "Prepper/GamePlay/Character/Component/BCombatComponent.h"
#include "Prepper/GamePlay/CharacterController/StatusComponent.h"
#include "Prepper/GamePlay/UI/PlayerOverlay.h"
#include "Prepper/Unreal/Controller/PlayerController/UnrealPlayerController.h"
#include "Prepper/___Legacy/HUD/PrepperHUD.h"
#include "Prepper/___Legacy/HUD/UI/CharacterOverlay/WeaponWidget.h"


// Sets default values for this component's properties
UInitialSettingComponent::UInitialSettingComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	// ...
}

void UInitialSettingComponent::Attach()
{
	UE_LOG(LogTemp, Warning, TEXT("WidgetAttach"));
	if (PlayerOverlay != nullptr)
	{
		PlayerOverlay->SetHUD(TargetCC->GetHUD<APrepperHUD>());
		TargetCC->GetTargetCharacter()->GetStatus()->Attach(PlayerOverlay);
		TargetCC->GetTargetCharacter()->GetCombat()->SetCrosshairOverlay(PlayerOverlay);
	}
	if (WeaponOverlay != nullptr)
	{
		TargetCC->GetTargetCharacter()->GetCombat()->Attach(WeaponOverlay);
	}
}

void UInitialSettingComponent::Detach()
{
	if (PlayerOverlay != nullptr)
	{
		TargetCC->GetTargetCharacter()->GetStatus()->Detach(PlayerOverlay);
	}
	if (WeaponOverlay != nullptr)
	{
		TargetCC->GetTargetCharacter()->GetCombat()->Detach(WeaponOverlay);
	}
	
}

void UInitialSettingComponent::WidgetSetting(TObjectPtr<AUnrealPlayerController> Target)
{
	if (PlayerOverlayClass)
	{
		PlayerOverlay = CreateWidget<UPlayerOverlay>(Target, PlayerOverlayClass);
		PlayerOverlay->AddToViewport();
		
	}
	
	if (WeaponOverlayClass)
	{
		WeaponOverlay = CreateWidget<UWeaponWidget>(Target, WeaponOverlayClass);
		WeaponOverlay->AddToViewport();
	}

	if (TargetCC != nullptr)
	{
		Attach();
	}
}

void UInitialSettingComponent::Initial(TObjectPtr<AUnrealPlayerController> Target)
{
	
	if (TargetCC != nullptr)
	{
		Detach();
	}
	
	TargetCC = Target;

	if (TargetCC != nullptr)
	{
		Attach();
	}
	
}
