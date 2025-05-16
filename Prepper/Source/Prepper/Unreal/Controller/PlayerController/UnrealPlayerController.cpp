// Fill out your copyright notice in the Description page of Project Settings.
#include "UnrealPlayerController.h"

#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "UnrealInitialSettingComponent.h"
#include "UnrealPlayerInputComponent.h"
#include "UnrealTabActionComponent.h"
#include "Prepper/GamePlay/Character/BCharacter.h"
#include "Prepper/Unreal/Controller/AmmoBox/UnrealAmmoBoxComponent.h"

AUnrealPlayerController::AUnrealPlayerController()
{
	InputConnector = CreateDefaultSubobject<UUnrealPlayerInputComponent>(TEXT("InputComponent"));
	TabAction = CreateDefaultSubobject<UUnrealTabActionComponent>(TEXT("TabActionComponent"));
	InitialSetting = CreateDefaultSubobject<UUnrealInitialSettingComponent>(TEXT("InitialSettingComponent"));
	
	AmmoBox = CreateDefaultSubobject<UUnrealAmmoBoxComponent>(TEXT("AmmoBoxComponent"));
}

TObjectPtr<ABCharacter> AUnrealPlayerController::GetTargetCharacter()
{
	if (TargetCharacter == nullptr)
	{
		TargetCharacter = GetPawn<ABCharacter>();
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

void AUnrealPlayerController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	if (GetTargetCharacter() == nullptr) return;
	
	InitialSetting->Initial(this);
	GetTargetCharacter()->SetAmmoBox(AmmoBox);
	TargetCharacter->SetTeamIdx(0);
}

void AUnrealPlayerController::OnRep_Pawn()
{
	Super::OnRep_Pawn();

	if (GetTargetCharacter() == nullptr) return;

	InitialSetting->Initial(this);
	GetTargetCharacter()->SetAmmoBox(AmmoBox);
	TargetCharacter->SetTeamIdx(0);
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

	InitialSetting->WidgetSetting(this);
	
	if (UEnhancedInputLocalPlayerSubsystem* Subsystem =
		ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(
		GetLocalPlayer()))
	{
		Subsystem->AddMappingContext(InputConnector->GetMappingContext(), 0);
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