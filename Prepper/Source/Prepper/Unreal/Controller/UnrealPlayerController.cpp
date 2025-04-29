// Fill out your copyright notice in the Description page of Project Settings.
#include "UnrealPlayerController.h"

#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "UnrealPlayerInputComponent.h"
#include "UnrealTabActionComponent.h"
#include "Prepper/GamePlay/Character/BCharacter.h"
#include "Prepper/GamePlay/Character/BCombatComponent.h"
#include "Prepper/GamePlay/Character/StatusComponent.h"
#include "Prepper/GamePlay/UI/PlayerOverlay.h"
#include "Prepper/__Legacy/HUD/UI/CharacterOverlay/WeaponWidget.h"

AUnrealPlayerController::AUnrealPlayerController()
{
	InputConnector = CreateDefaultSubobject<UUnrealPlayerInputComponent>(TEXT("InputComponent"));
	TabAction = CreateDefaultSubobject<UUnrealTabActionComponent>(TEXT("TabActionComponent"));
	
}

TObjectPtr<ABCharacter> AUnrealPlayerController::GetTargetCharacter()
{
	if (TargetCharacter == nullptr)
	{
		TargetCharacter = GetPawn<ABCharacter>();
		TargetCharacter->SetTeamIdx(0);
	}
	return TargetCharacter;
}

void AUnrealPlayerController::CrouchingAct(bool IsTrigger)
{
	const TObjectPtr<ABCharacter> Target = GetTargetCharacter();
	if (Target == nullptr) return;
	if (IsTrigger)
	{
		Target->Crouch();
		return;
	}
	Target->UnCrouch();
}

void AUnrealPlayerController::JumpAct(bool IsTrigger)
{
	const TObjectPtr<ABCharacter> Target = GetTargetCharacter();
	if (Target == nullptr) return;
	if (IsTrigger)
	{
		Target->Jump();
		return;
	}
	Target->StopJumping();
}


void AUnrealPlayerController::SprintAct(bool IsTrigger)
{
	const TObjectPtr<ABCharacter> Target = GetTargetCharacter();
	if (Target == nullptr) return;
	Target->SprintTrigger(IsTrigger);
}

void AUnrealPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();
	
	if (const TObjectPtr<UEnhancedInputComponent> EnhancedInputComponent
		= Cast<UEnhancedInputComponent>(InputComponent))
	{
		InputConnector->SetInput(this, EnhancedInputComponent);
	}
}

void AUnrealPlayerController::BeginPlay()
{
	Super::BeginPlay();
	
	if (!IsLocalController()) return;
	
	if (UEnhancedInputLocalPlayerSubsystem* Subsystem =
		ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(
		GetLocalPlayer()))
	{
		Subsystem->AddMappingContext(InputConnector->GetMappingContext(), 0);
	}
	
	if (PlayerOverlayClass)
	{
		PlayerOverlay = CreateWidget<UPlayerOverlay>(this, PlayerOverlayClass);
		PlayerOverlay->AddToViewport();
		
		GetTargetCharacter()->GetStatus()->Attach(PlayerOverlay);
		
	}
	
	if (WeaponOverlayClass)
	{
		WeaponOverlay = CreateWidget<UWeaponWidget>(this, WeaponOverlayClass);
		WeaponOverlay->AddToViewport();

		GetTargetCharacter()->GetCombat()->Attach(WeaponOverlay);
	}

}

void AUnrealPlayerController::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
}

void AUnrealPlayerController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	//PlayerOverlay->DrawCrosshair();
}

void AUnrealPlayerController::TabButtonPressed()
{
	TabAction->TabPressed();
}

void AUnrealPlayerController::TabButtonReleased()
{
	TabAction->TabReleased();
}