// Fill out your copyright notice in the Description page of Project Settings.
#include "UnrealPlayerController.h"

#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "PlayerInputComponent.h"
#include "UnrealStatusComponent.h"
#include "UnrealCombatComponent.h"
#include "UnrealInteractionComponent.h"
#include "Net/UnrealNetwork.h"
#include "Prepper/GamePlay/BCharacter.h"
#include "Prepper/__Legacy/Interfaces/Interactable.h"

AUnrealPlayerController::AUnrealPlayerController()
{
	IsCrouching = false;
	IsSprint = false;
	IsJump = false;
	IsCrouchingLocal = false;
	IsSprintLocal = false;
	
	InputConnector = CreateDefaultSubobject<UPlayerInputComponent>(TEXT("InputComponent"));
	Status = CreateDefaultSubobject<UUnrealStatusComponent>(TEXT("StatusComponent"));
	Combat = CreateDefaultSubobject<UUnrealCombatComponent>(TEXT("CombatComponent"));
	Interaction = CreateDefaultSubobject<UUnrealInteractionComponent>(TEXT("InteractionComponent"));
	
}

void AUnrealPlayerController::OnRep_Crouching()
{
	if (IsLocalController()) return;
	CrouchingAct(IsCrouching);
}

void AUnrealPlayerController::OnRep_Sprint()
{
	if (IsLocalController()) return;
	SprintAct(IsSprint);
}

void AUnrealPlayerController::OnRep_Jump()
{
	if (IsLocalController()) return;
	JumpAct(IsJump);
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
	
	if (UEnhancedInputLocalPlayerSubsystem* Subsystem =
		ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(
		GetLocalPlayer()))
	{
		Subsystem->AddMappingContext(InputConnector->GetMappingContext(), 0);
	}

	Combat->SetTargetCC(this);
	Interaction->SetTargetCC(this);

}

void AUnrealPlayerController::PlayAnim(const FString& String)
{
	UE_LOG(LogTemp, Warning, TEXT("%s, %s"), *TargetCharacter->GetName(), *String);
}

void AUnrealPlayerController::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AUnrealPlayerController, IsCrouching);
	DOREPLIFETIME(AUnrealPlayerController, IsSprint);
	DOREPLIFETIME(AUnrealPlayerController, IsJump);
}

IStatus* AUnrealPlayerController::GetStatus()
{
	return Status;
}

ICombat* AUnrealPlayerController::GetCombat()
{
	return Combat;
}

void AUnrealPlayerController::Move(const FInputActionValue& Value)
{
	const TObjectPtr<ABCharacter> Target = GetTargetCharacter();
	if (Target == nullptr) return;
	
	const FVector2D MovementVector = Value.Get<FVector2D>();
	Target->Move(MovementVector.X, MovementVector.Y);
}

void AUnrealPlayerController::Look(const FInputActionValue& Value)
{
	const TObjectPtr<ABCharacter> Target = GetTargetCharacter();
	if (Target == nullptr) return;
	
	const FVector2D LookAxisVector = Value.Get<FVector2D>();
	Target->Look(LookAxisVector.X, LookAxisVector.Y);
}

void AUnrealPlayerController::CrouchToggle()
{
	IsCrouchingLocal = !IsCrouchingLocal;
	ServerCrouchTrigger(IsCrouchingLocal);
	CrouchingAct(IsCrouchingLocal);
}

void AUnrealPlayerController::JumpTrigger(bool IsTrigger)
{
	ServerJumpTrigger(IsTrigger);
	JumpAct(IsTrigger);
}

void AUnrealPlayerController::SprintTrigger(bool IsTrigger)
{
	IsSprintLocal = IsTrigger;
	ServerSprintTrigger(IsSprintLocal);
	SprintAct(IsSprintLocal);
}

void AUnrealPlayerController::EquipButtonPressed()
{
	ServerEquipButtonPressed();
}

TObjectPtr<ABCharacter> AUnrealPlayerController::GetTargetCharacter()
{
	if (TargetCharacter == nullptr)
	{
		TargetCharacter = GetPawn<ABCharacter>();
	}
	return TargetCharacter;
}

void AUnrealPlayerController::ServerJumpTrigger_Implementation(bool IsTrigger)
{
	IsJump = IsTrigger;
}

void AUnrealPlayerController::ServerSprintTrigger_Implementation(bool IsTrigger)
{
	IsSprint = IsTrigger;
}

void AUnrealPlayerController::ServerCrouchTrigger_Implementation(bool IsTrigger)
{
	IsCrouching = IsTrigger;
}

void AUnrealPlayerController::ServerEquipButtonPressed_Implementation()
{
	TScriptInterface<IInteractable> Target = Interaction->GetInteractable();
	if (Target == nullptr) return;
	
	Target->Interaction(this);
}