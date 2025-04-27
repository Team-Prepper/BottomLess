// Fill out your copyright notice in the Description page of Project Settings.
#include "UnrealPlayerController.h"

#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Component/UnrealPlayerInputComponent.h"
#include "Component/UnrealStatusComponent.h"
#include "Component/UnrealCombatComponent.h"
#include "Component/UnrealTabActionComponent.h"
#include "Component/UnrealInteractionComponent.h"
#include "Net/UnrealNetwork.h"
#include "Prepper/GamePlay/Character/BCharacter.h"
#include "Prepper/GamePlay/UI/PlayerOverlay.h"
#include "Prepper/__Legacy/Interfaces/Interactable.h"

AUnrealPlayerController::AUnrealPlayerController()
{
	IsCrouching = false;
	IsSprint = false;
	IsJump = false;
	IsCrouchingLocal = false;
	IsSprintLocal = false;
	
	InputConnector = CreateDefaultSubobject<UUnrealPlayerInputComponent>(TEXT("InputComponent"));
	Status = CreateDefaultSubobject<UUnrealStatusComponent>(TEXT("StatusComponent"));
	Combat = CreateDefaultSubobject<UUnrealCombatComponent>(TEXT("CombatComponent"));
	Interaction = CreateDefaultSubobject<UUnrealInteractionComponent>(TEXT("InteractionComponent"));
	TabAction = CreateDefaultSubobject<UUnrealTabActionComponent>(TEXT("TabActionComponent"));
	
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

TObjectPtr<ABCharacter> AUnrealPlayerController::GetTargetCharacter()
{
	if (TargetCharacter == nullptr)
	{
		TargetCharacter = GetPawn<ABCharacter>();
	}
	return TargetCharacter;
}

IStatus* AUnrealPlayerController::GetStatus()
{
	return Status;
}

ICombat* AUnrealPlayerController::GetCombat()
{
	return Combat;
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
	
	if (PlayerOverlayClass)
	{
		PlayerOverlay = CreateWidget<UPlayerOverlay>(this, PlayerOverlayClass);
		PlayerOverlay->AddToViewport();
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

void AUnrealPlayerController::TabButtonPressed()
{
	TabAction->TabPressed();
}

void AUnrealPlayerController::TabButtonReleased()
{
	TabAction->TabReleased();
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
	if (Target == nullptr)
	{
		Combat->Swap();
		return;
	}
	
	Target->Interaction(this);
}