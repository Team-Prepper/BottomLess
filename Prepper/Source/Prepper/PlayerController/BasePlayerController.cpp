// Fill out your copyright notice in the Description page of Project Settings.


#include "BasePlayerController.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "GameFramework/PlayerState.h"
#include "Prepper/HUD/PrepperHUD.h"


void ABasePlayerController::BeginPlay()
{
	Super::BeginPlay();
	if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
	{
		Subsystem->AddMappingContext(PlayerMappingContext, 0);
	}
}

void ABasePlayerController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
	PlayerCharacter = Cast<APlayerCharacter>(InPawn);
	
	if (Cast<IControllable>(GetPawn()))
	{
		TargetControllerable = GetPawn();
	}
}

void ABasePlayerController::PollInit()
{
	if (Cast<IControllable>(GetPawn()))
	{
		TargetControllerable = GetPawn();
	}
}
void ABasePlayerController::ServerReportPingStatus_Implementation(bool bHighPing)
{
	HighPingDelegate.Broadcast(bHighPing);
}

void ABasePlayerController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	PollInit();
	CheckPing(DeltaTime);
}

void ABasePlayerController::CheckPing(float DeltaTime)
{
	HighPingRunningTime += DeltaTime;
	if(HighPingRunningTime > CheckPingFrequency)
	{
		if(!PlayerState)
		{
			PlayerState = GetPlayerState<APlayerState>();
		}
		UE_LOG(LogTemp,Warning,TEXT("Ping chk"));
		FString ping = FString::SanitizeFloat(PlayerState->GetPingInMilliseconds());
		UE_LOG(LogTemp,Warning,TEXT("PING : %s"), *ping);
		
		if(PlayerState)
		{
			if(PlayerState->GetPingInMilliseconds() > HighPingThreshold)
			{
				UE_LOG(LogTemp,Warning,TEXT("HIGH PING WARNING"));
				if(IsLocalController())
				{
					HighPingWarningBP();
					PingAnimationRunningTime = 0.f;
				}
			}
		}
		HighPingRunningTime = 0.f;
		
	}
}

void ABasePlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(InputComponent))
	{
		// Moving
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ABasePlayerController::Move);
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Completed, this, &ABasePlayerController::Move);
		// Looking
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &ABasePlayerController::Look);

		// Jumping
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &ABasePlayerController::JumpButtonPressed);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ABasePlayerController::JumpButtonReleased);

		//Sprint
		EnhancedInputComponent->BindAction(SprintAction, ETriggerEvent::Triggered, this, &ABasePlayerController::SprintButtonPressed);
		EnhancedInputComponent->BindAction(SprintAction, ETriggerEvent::Completed, this, &ABasePlayerController::SprintButtonReleased);

		// Equip
		EnhancedInputComponent->BindAction(EquipAction, ETriggerEvent::Triggered, this, &ABasePlayerController::EquipButtonPressed);

		// Crouch
		EnhancedInputComponent->BindAction(CrouchAction, ETriggerEvent::Triggered, this, &ABasePlayerController::CrouchButtonPressed);

		//Aim
		EnhancedInputComponent->BindAction(AimAction, ETriggerEvent::Triggered, this, &ABasePlayerController::AimButtonPressed);
		EnhancedInputComponent->BindAction(AimAction, ETriggerEvent::Completed, this, &ABasePlayerController::AimButtonReleased);

		// Fire
		EnhancedInputComponent->BindAction(FireAction, ETriggerEvent::Triggered, this, &ABasePlayerController::FireButtonPressed);
		EnhancedInputComponent->BindAction(FireAction, ETriggerEvent::Completed, this, &ABasePlayerController::FireButtonReleased);

		// Reload
		EnhancedInputComponent->BindAction(ReloadAction, ETriggerEvent::Triggered, this, &ABasePlayerController::ReloadButtonPressed);

		EnhancedInputComponent->BindAction(OpenInventory, ETriggerEvent::Triggered, this, &ABasePlayerController::OpenInventoryPressed);
	}
}


void ABasePlayerController::Move(const FInputActionValue& Value)
{
	if (!TargetControllerable) return;
	TargetControllerable->Move(Value);
	
}
void ABasePlayerController::Look(const FInputActionValue& Value)
{
	if (!TargetControllerable) return;
	TargetControllerable->Look(Value);
}
void ABasePlayerController::JumpButtonPressed()
{
	if (!TargetControllerable) return;
	TargetControllerable->SpacePressed();
}
void ABasePlayerController::JumpButtonReleased()
{
	if (!TargetControllerable) return;
	TargetControllerable->SpaceReleased();
}
void ABasePlayerController::SprintButtonPressed()
{
	if (!TargetControllerable) return;
	TargetControllerable->ShiftPressed();
}
void ABasePlayerController::SprintButtonReleased()
{
	if (!TargetControllerable) return;
	TargetControllerable->ShiftReleased();
}
void ABasePlayerController::EquipButtonPressed()
{
	ServerInteractionPressed();
}
void ABasePlayerController::CrouchButtonPressed()
{
	if (!TargetControllerable) return;
	TargetControllerable->ControlPressed();
	
}
void ABasePlayerController::ReloadButtonPressed()
{
	if (!TargetControllerable) return;
	TargetControllerable->RPressed();
}
void ABasePlayerController::AimButtonPressed()
{
	if (!TargetControllerable) return;
	TargetControllerable->MouseRightPressed();
}
void ABasePlayerController::AimButtonReleased()
{
	if (!TargetControllerable) return;
	TargetControllerable->MouseRightReleased();
	
}
void ABasePlayerController::FireButtonPressed()
{
	if (!TargetControllerable) return;
	TargetControllerable->MouseLeftPressed();
}
void ABasePlayerController::FireButtonReleased()
{
	if (!TargetControllerable) return;
	TargetControllerable->MouseLeftReleased();
}

void ABasePlayerController::OpenInventoryPressed()
{
	if (!PrepperHUD) return;
	PrepperHUD->ToggleInventory();
}

void ABasePlayerController::ServerInteractionPressed_Implementation()
{
	if (!TargetControllerable) return;
	TargetControllerable->EPressed();
}

void ABasePlayerController::BindPlayerAction()
{
	if (Cast<IControllable>(GetPawn()))
	{
		TargetControllerable = GetPawn();
	}
}