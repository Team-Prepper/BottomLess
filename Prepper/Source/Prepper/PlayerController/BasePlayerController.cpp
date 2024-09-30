// Fill out your copyright notice in the Description page of Project Settings.


#include "BasePlayerController.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "GameFramework/PlayerState.h"
#include "Prepper/HUD/PrepperHUD.h"
#include "Prepper/HUD/UI/Compass.h"


void ABasePlayerController::BeginPlay()
{
	Super::BeginPlay();
	
	if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(
		GetLocalPlayer()))
	{
		Subsystem->AddMappingContext(PlayerMappingContext, 0);
	}

	if (!IsLocalController()) return;
	
	SetInputMode(FInputModeGameOnly());
	BeginWidget();

	PossessPlayerCharacter();

}

void ABasePlayerController::BeginWidget()
{
	if (CharacterOverlayClass)
	{
		CharacterOverlay = CreateWidget<UCharacterOverlay>(this, CharacterOverlayClass);
		CharacterOverlay->AddToViewport();
	}
	
	if (CompassHUDClass)
	{
		Compass = CreateWidget<UCompass>(this, CompassHUDClass);
		Compass->AddToViewport();
		if (TargetControlMapper)
		{
			Compass->SetTargetCamera(TargetControlMapper->GetFollowCamera());
		}
	}
}

void ABasePlayerController::OnPossess(APawn* InPawn)
{
	// 서버에서만 동작하는 함수
	Super::OnPossess(InPawn);
	OnPossess();

}

void ABasePlayerController::OnRep_Pawn()
{
	Super::OnRep_Pawn();
	OnPossess();
	
}

void ABasePlayerController::OnPossess()
{
	if (!GetPawn()) return;
	
	UE_LOG(LogTemp, Warning, TEXT("Pawn %s possessed by PlayerController %s"),
		*GetPawn()->GetName(), *GetName());
	UE_LOG(LogTemp, Warning, TEXT("Testing"));
	
	// 로컬에서도 동작하게 설계함
	if (Cast<IControllable>(GetPawn()))
	{
		if (TargetControlMapper && IsLocalController())
		{
			TargetControlMapper->Disconnect();
		}
		TargetControlMapper = Cast<IControllable>(GetPawn())->GetControlMapper();
	}

	if (!IsLocalController()) return;

	TargetControlMapper->Connect(this);

	if (Compass)
	{
		Compass->SetTargetCamera(TargetControlMapper->GetFollowCamera());
		UE_LOG(LogTemp, Warning, TEXT("[PrepperPlayerController] : Set Compass"));
	}
	
	if (Cast<APlayerCharacter>(GetPawn()))
	{
		PlayerCharacter = Cast<APlayerCharacter>(GetPawn());
	}

	if (!PlayerCharacter) return;

	PossessPlayerCharacter();
}

void ABasePlayerController::PlayerTick(float DeltaTime)
{
	Super::PlayerTick(DeltaTime);
	
	if (Compass)
		Compass->SetDirection();
}

void ABasePlayerController::PossessPlayerCharacter()
{
	if (!CharacterOverlay) return;
	if (!PlayerCharacter) return;
	
	PlayerCharacter->Attach(CharacterOverlay);
}

void ABasePlayerController::ServerReportPingStatus_Implementation(bool bHighPing)
{
	HighPingDelegate.Broadcast(bHighPing);
}

APrepperHUD* ABasePlayerController::GetPrepperHUD()
{
	if (PrepperHUD) return PrepperHUD;
	
	PrepperHUD = Cast<APrepperHUD>(GetHUD());

	if (PrepperHUD == nullptr) return nullptr;

	PrepperHUD->ResetCrossHair();
	
	return PrepperHUD;
}

void ABasePlayerController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	CheckPing(DeltaTime);
}

void ABasePlayerController::CheckPing(float DeltaTime)
{
	HighPingRunningTime += DeltaTime;
	if (HighPingRunningTime <= CheckPingFrequency) return;

	if (!PlayerState)
	{
		PlayerState = GetPlayerState<APlayerState>();
	}

	UE_LOG(LogTemp, Warning, TEXT("Ping chk"));
	FString ping = FString::SanitizeFloat(PlayerState->GetPingInMilliseconds());
	UE_LOG(LogTemp, Warning, TEXT("PING : %s"), *ping);

	if (PlayerState && PlayerState->GetPingInMilliseconds() > HighPingThreshold)
	{
		UE_LOG(LogTemp, Warning, TEXT("HIGH PING WARNING"));
		if (IsLocalController())
		{
			HighPingWarningBP();
			PingAnimationRunningTime = 0.f;
		}
	}
	HighPingRunningTime = 0.f;
}

void ABasePlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(InputComponent))
	{
		SetInput(EnhancedInputComponent);
	}
}


void ABasePlayerController::SetInput(UEnhancedInputComponent* Input)
{
	// Moving
	Input->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ABasePlayerController::Move);
	Input->BindAction(MoveAction, ETriggerEvent::Completed, this, &ABasePlayerController::Move);
	
	// Looking
	Input->BindAction(LookAction, ETriggerEvent::Triggered, this, &ABasePlayerController::Look);

	// Jumping
	Input->BindAction(JumpAction, ETriggerEvent::Started, this, &ABasePlayerController::JumpButtonPressed);
	Input->BindAction(JumpAction, ETriggerEvent::Completed, this, &ABasePlayerController::JumpButtonReleased);

	//Sprint
	Input->BindAction(SprintAction, ETriggerEvent::Triggered, this, &ABasePlayerController::SprintButtonPressed);
	Input->BindAction(SprintAction, ETriggerEvent::Completed, this, &ABasePlayerController::SprintButtonReleased);

	// Equip
	Input->BindAction(EquipAction, ETriggerEvent::Triggered, this, &ABasePlayerController::EquipButtonPressed);

	// Crouch
	Input->BindAction(CrouchAction, ETriggerEvent::Triggered, this, &ABasePlayerController::CrouchButtonPressed);

	//Aim
	Input->BindAction(AimAction, ETriggerEvent::Triggered, this, &ABasePlayerController::AimButtonPressed);
	Input->BindAction(AimAction, ETriggerEvent::Completed, this, &ABasePlayerController::AimButtonReleased);

	// Fire
	Input->BindAction(FireAction, ETriggerEvent::Triggered, this, &ABasePlayerController::FireButtonPressed);
	Input->BindAction(FireAction, ETriggerEvent::Completed, this, &ABasePlayerController::FireButtonReleased);

	// Reload
	Input->BindAction(ReloadAction, ETriggerEvent::Triggered, this, &ABasePlayerController::ReloadButtonPressed);
	
}

void ABasePlayerController::Move(const FInputActionValue& Value)
{
	if (!TargetControlMapper) return;
	TargetControlMapper->Move(Value);
}

void ABasePlayerController::Look(const FInputActionValue& Value)
{
	if (!TargetControlMapper) return;
	TargetControlMapper->Look(Value);
}

void ABasePlayerController::JumpButtonPressed()
{
	if (!TargetControlMapper) return;
	TargetControlMapper->SpacePressed();
}

void ABasePlayerController::JumpButtonReleased()
{
	if (!TargetControlMapper) return;
	TargetControlMapper->SpaceReleased();
}

void ABasePlayerController::SprintButtonPressed()
{
	if (!TargetControlMapper) return;
	TargetControlMapper->ShiftPressed();
}

void ABasePlayerController::SprintButtonReleased()
{
	if (!TargetControlMapper) return;
	TargetControlMapper->ShiftReleased();
}

void ABasePlayerController::EquipButtonPressed()
{
	ServerInteractionPressed();
}

void ABasePlayerController::CrouchButtonPressed()
{
	if (!TargetControlMapper) return;
	TargetControlMapper->ControlPressed();
}

void ABasePlayerController::ReloadButtonPressed()
{
	if (!TargetControlMapper) return;
	TargetControlMapper->RPressed();
}

void ABasePlayerController::AimButtonPressed()
{
	if (!TargetControlMapper) return;
	TargetControlMapper->MouseRightPressed();
}

void ABasePlayerController::AimButtonReleased()
{
	if (!TargetControlMapper) return;
	TargetControlMapper->MouseRightReleased();
}

void ABasePlayerController::FireButtonPressed()
{
	if (!TargetControlMapper) return;
	TargetControlMapper->MouseLeftPressed();
}

void ABasePlayerController::FireButtonReleased()
{
	if (!TargetControlMapper) return;
	TargetControlMapper->MouseLeftReleased();
}

void ABasePlayerController::ServerInteractionPressed_Implementation()
{
	if (!TargetControlMapper) return;
	TargetControlMapper->EPressed();
}
