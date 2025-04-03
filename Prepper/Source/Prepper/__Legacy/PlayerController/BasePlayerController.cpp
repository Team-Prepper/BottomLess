// Fill out your copyright notice in the Description page of Project Settings.


#include "BasePlayerController.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "GameFramework/PlayerState.h"
#include "Prepper/__Legacy/HUD/PrepperHUD.h"
#include "Prepper/__Legacy/HUD/UI/Compass.h"


void ABasePlayerController::BeginPlay()
{
	Super::BeginPlay();
	
	if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(
		GetLocalPlayer()))
	{
		Subsystem->AddMappingContext(PlayerMappingContext, 0);
	}


	if (IsLocalController())
	{
		SetInputMode(FInputModeGameOnly());
		BeginWidget();
	}
	
	LocalPossessNewPlayerCharacter();

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
	
	if (SettingClass)
	{
		Setting = CreateWidget<UUserWidget>(this, SettingClass);
		Setting->SetVisibility(ESlateVisibility::Hidden);
	}
	
	if (DeathWidgetClass)
	{
		DeathWidget = CreateWidget<UUserWidget>(this, DeathWidgetClass);
		DeathWidget->SetVisibility(ESlateVisibility::Hidden);
	}
}

void ABasePlayerController::ServerPossessNewPlayerCharacter()
{
}

void ABasePlayerController::OnPossess(APawn* InPawn)
{
	// 서버에서만 동작하는 함수
	Super::OnPossess(InPawn);

}

void ABasePlayerController::OnRep_Pawn()
{
	Super::OnRep_Pawn();
	
}
void ABasePlayerController::PlayerTick(float DeltaTime)
{
	Super::PlayerTick(DeltaTime);
	
	if (Compass)
		Compass->SetDirection();
}

TObjectPtr<APlayerCharacter> ABasePlayerController::GetPlayerCharacter()
{
	return PlayerCharacter;
}

void ABasePlayerController::LocalPossessNewPlayerCharacter()
{
	if (!CharacterOverlay) return;
	if (!PlayerCharacter) return;
	
	PlayerCharacter->Attach(CharacterOverlay);
	
	if (DeathWidget == nullptr) return;
	
	SetInputMode(FInputModeGameOnly());
	SetShowMouseCursor(false);
	DeathWidget->SetVisibility(ESlateVisibility::Hidden);
	DeathWidget->RemoveFromParent();

}

void ABasePlayerController::MulticastElim_Implementation()
{
	PlayerCharacter = nullptr;
	
	if (!IsLocalController()) return;
	if (DeathWidget == nullptr) return;
	
	SetInputMode(FInputModeUIOnly());
	SetShowMouseCursor(true);
	DeathWidget->SetVisibility(ESlateVisibility::Visible);
	DeathWidget->AddToViewport();
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

void ABasePlayerController::Elim()
{
	MulticastElim();
}

void ABasePlayerController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	CheckPing(DeltaTime);
}

void ABasePlayerController::SetPawn(APawn* InPawn)
{
	Super::SetPawn(InPawn);

	if (GetPawn() == nullptr) return;

	UE_LOG(LogTemp, Warning, TEXT("Try but: %s"), PlayerCharacter == nullptr ? *FString("null") : *PlayerCharacter->GetName());
	//UE_LOG(LogTemp, Warning, TEXT("Pawn %s possessed by PlayerController %s"),
	//	*GetPawn()->GetName(), *GetName());
	
	// 로컬에서도 동작하게 설계함
	if (IControllable* Controllable = GetPawn<IControllable>())
	{
		if (TargetControlMapper && IsLocalController())
		{
			TargetControlMapper->Disconnect();
		}
		TargetControlMapper = Controllable->GetControlMapper();
	}

	if (IsLocalController())
	{
		TargetControlMapper->Connect(this);

		if (Compass != nullptr && TargetControlMapper != nullptr)
		{
			Compass->SetTargetCamera(TargetControlMapper->GetFollowCamera());
			//UE_LOG(LogTemp, Warning, TEXT("[PrepperPlayerController] : Set Compass"));
		}
	}
	if (PlayerCharacter != nullptr) return;
	
	TObjectPtr<APlayerCharacter> NewPlayer = GetPawn<APlayerCharacter>();
	
	if (HasAuthority())
	{
		PlayerCharacter = NewPlayer;
		ServerPossessNewPlayerCharacter();
	}

	if (!IsLocalController()) return;
	
	PlayerCharacter = NewPlayer;
	LocalPossessNewPlayerCharacter();
	
}

void ABasePlayerController::CheckPing(float DeltaTime)
{
	HighPingRunningTime += DeltaTime;
	if (HighPingRunningTime <= CheckPingFrequency) return;

	if (!PlayerState)
	{
		PlayerState = GetPlayerState<APlayerState>();
	}

	//UE_LOG(LogTemp, Warning, TEXT("Ping chk"));
	FString ping = FString::SanitizeFloat(PlayerState->GetPingInMilliseconds());
	//UE_LOG(LogTemp, Warning, TEXT("PING : %s"), *ping);

	if (PlayerState && PlayerState->GetPingInMilliseconds() > HighPingThreshold)
	{
		//UE_LOG(LogTemp, Warning, TEXT("HIGH PING WARNING"));
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
	
	Input->BindAction(OpenSetting, ETriggerEvent::Completed, this, &ABasePlayerController::OpenSettingWidget);
	
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

void ABasePlayerController::OpenSettingWidget()
{
	if (!Setting) return;
	if (Setting->GetVisibility() == ESlateVisibility::Hidden)
	{
		SetInputMode(FInputModeGameAndUI());
		SetShowMouseCursor(true);
		Setting->AddToViewport();
		Setting->SetVisibility(ESlateVisibility::Visible);
		return;
	}
	SetInputMode(FInputModeGameOnly());
	SetShowMouseCursor(false);
	Setting->SetVisibility(ESlateVisibility::Hidden);
	Setting->RemoveFromParent();
}

void ABasePlayerController::CloseSettingWidget()
{
	if (!Setting) return;
	Setting->SetVisibility(ESlateVisibility::Hidden);
}

void ABasePlayerController::ServerInteractionPressed_Implementation()
{
	if (!TargetControlMapper) return;
	TargetControlMapper->EPressed();
}
