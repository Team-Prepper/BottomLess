// Fill out your copyright notice in the Description page of Project Settings.
#include "UnrealPlayerController.h"

#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "UnrealPlayerInputComponent.h"
#include "Prepper/GamePlay/Character/BCharacter.h"
#include "Prepper/GamePlay/CharacterController/EscapeActionComponent.h"
#include "Prepper/GamePlay/CharacterController/GameSaveComponent.h"
#include "Prepper/GamePlay/CharacterController/InitialSettingComponent.h"
#include "Prepper/GamePlay/CharacterController/TabActionComponent.h"
#include "Prepper/Unreal/Controller/AmmoBox/UnrealAmmoBoxComponent.h"

AUnrealPlayerController::AUnrealPlayerController()
{
	InputConnector = CreateDefaultSubobject<UUnrealPlayerInputComponent>(TEXT("InputComponent"));
	TabAction = CreateDefaultSubobject<UTabActionComponent>(TEXT("TabActionComponent"));
	InitialSetting = CreateDefaultSubobject<UInitialSettingComponent>(TEXT("InitialSettingComponent"));
	EscapeAction = CreateDefaultSubobject<UEscapeActionComponent>(TEXT("EscapeActionComponent"));
	
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

TObjectPtr<ACar> AUnrealPlayerController::GetTargetCar()
{
	return TargetCar;
}

void AUnrealPlayerController::OnPossess(APawn* InPawn)
{
	UE_LOG(LogTemp, Warning, TEXT("Possess"));
	Super::OnPossess(InPawn);
	
	SetViewTarget(InPawn);

	if (GetPawn<ACar>() != nullptr)
	{
		TargetCar = GetPawn<ACar>();
		return;
	}
	
	TargetCar = nullptr;
	
	if (GetPawn() == nullptr)
	{
		TargetCharacter = nullptr;
		return;
	}
	
	if (TargetCharacter == GetPawn()) return;

	TargetCharacter = GetPawn<ABCharacter>();
	
	InitialSetting->Initial(this);
	GetTargetCharacter()->SetAmmoBox(AmmoBox);
	TargetCharacter->SetTeamIdx(0);
	
}

void AUnrealPlayerController::OnRep_Pawn()
{
	Super::OnRep_Pawn();
	
	SetViewTarget(GetPawn());
	
	if (GetPawn<ACar>() != nullptr)
	{
		TargetCar = GetPawn<ACar>();
		return;
	}

	TargetCar = nullptr;

	if (GetPawn() == nullptr)
	{
		TargetCharacter = nullptr;
		return;
	}

	if (TargetCharacter == GetPawn()) return;
	
	TargetCharacter = GetPawn<ABCharacter>();
	
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

	InputConnector->SetCC(this);
	
	if (!IsLocalController()) return;

	InitialSetting->WidgetSetting(this);
	
	if (UEnhancedInputLocalPlayerSubsystem* Subsystem =
		ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(
		GetLocalPlayer()))
	{
		Subsystem->AddMappingContext(InputConnector->GetMappingContext(), 0);
	}
	
}

void AUnrealPlayerController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	//PlayerOverlay->DrawCrosshair();
}

void AUnrealPlayerController::TabButtonPressed() const
{
	TabAction->TabPressed();
}

void AUnrealPlayerController::TabButtonReleased() const
{
	TabAction->TabReleased();
}

void AUnrealPlayerController::EscapeButtonPressed() const
{
	EscapeAction->EscapePressed();
}

void AUnrealPlayerController::GameSaveAction() const
{
	GameSave->GameSave();
}