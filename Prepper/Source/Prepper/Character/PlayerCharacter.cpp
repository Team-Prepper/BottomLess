#include "PlayerCharacter.h"	
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"
#include "Components/CapsuleComponent.h"
#include "Components/WidgetComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Net/UnrealNetwork.h"
#include "Prepper/Prepper.h"
#include "Prepper/Component/CombatComponent.h"
#include "Prepper/GameMode/DeathMatchGameMode.h"
#include "Prepper/Item/AInteractable.h"
#include "Prepper/PlayerController/PrepperPlayerController.h"
#include "Prepper/PlayerState/DeathMatchPlayerState.h"
#include "Prepper/Weapon/Weapon.h"
#include "WorldPartition/ContentBundle/ContentBundleLog.h"


APlayerCharacter::APlayerCharacter()
{
	PrimaryActorTick.bCanEverTick = true;
	SpawnCollisionHandlingMethod = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
	CrouchCamOffset = 30.f;
	DefaultCamOffset = 100.f;
	CrouchCamArmLength = 200.f;
	DefaultCamArmLength = 350.f;
	InterpSpeed = 5.f;

	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(GetMesh());
	CameraBoom->TargetArmLength = DefaultCamArmLength;
	CameraBoom->bUsePawnControlRotation = true;

	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCam"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	FollowCamera->bUsePawnControlRotation = false;

	Hair = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Hair"));
	Hair->SetRelativeRotation(FRotator(-90.f, 0, 90.f));
	Hair->SetupAttachment(GetMesh(),FName("Head"));

	bUseControllerRotationYaw = false;
	GetCharacterMovement()->bOrientRotationToMovement = true;

	OverheadWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("OverheadWidget"));
	OverheadWidget->SetupAttachment(RootComponent);

	Combat = CreateDefaultSubobject<UCombatComponent>(TEXT("CombatComponent"));
	Combat->SetIsReplicated(true);

	GetCharacterMovement()->NavAgentProps.bCanCrouch = true;
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);

	GetMesh()->SetCollisionObjectType(ECC_SkeletalMesh);
	GetMesh()->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
	GetMesh()->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);
	GetCharacterMovement()->RotationRate = FRotator(0.f, 0.f, 850.f);

	TurningInPlace = ETurningInPlace::ETIP_NotTurning;

	NetUpdateFrequency = 66.f;
	MinNetUpdateFrequency = 33.f;

	beforeSeat = false;
}

void APlayerCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	DOREPLIFETIME_CONDITION(APlayerCharacter, OverlappingItem, COND_OwnerOnly);
	DOREPLIFETIME(APlayerCharacter, bDisableGamePlay);
}

void APlayerCharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	if(Combat)
	{
		Combat->Character = this;
	}
}

void APlayerCharacter::BeginPlay()
{
	Super::BeginPlay();
	
	PrepperPlayerController = Cast<APrepperPlayerController>(Controller);
	if(PrepperPlayerController)
	{
		PrepperPlayerController->BindPlayerAction();
	}
}

void APlayerCharacter::Tick(float DeltaTime)
{
	ACharacter::Tick(DeltaTime);

	RotateInPlace(DeltaTime);
	HideCamIfCharacterClose();

	FVector CurrentLocation = CameraBoom->GetRelativeLocation();
	FVector NewLocation = FMath::VInterpTo(CurrentLocation, TargetSpringArmLocation, DeltaTime, InterpSpeed);
	CameraBoom->SetRelativeLocation(NewLocation);
	
	float CurrentArmLength = CameraBoom->TargetArmLength;
	float NewArmLength = FMath::FInterpTo(CurrentArmLength, TargetArmLength, DeltaTime, InterpSpeed);
	CameraBoom->TargetArmLength = NewArmLength;
	
	PollInit();
}

void APlayerCharacter::RotateInPlace(float DeltaTime)
{
	if(bDisableGamePlay)
	{
		bUseControllerRotationYaw = false;
		TurningInPlace = ETurningInPlace::ETIP_NotTurning;
		return;
	}
	if(GetLocalRole() > ROLE_SimulatedProxy && IsLocallyControlled())
	{
		AimOffset(DeltaTime);
	}
	else
	{
		TimeSinceLastMovementReplication += DeltaTime;
		if (TimeSinceLastMovementReplication > 0.25f)
		{
			OnRep_ReplicatedMovement();
		}
		CalculateAO_Pitch();
	}
}


void APlayerCharacter::ShiftPressed()
{
	if(bDisableGamePlay) return;
	SetState("Sprint");
	ServerSetState("Sprint"); // server
}

void APlayerCharacter::ShiftReleased()
{
	if(bDisableGamePlay) return;
	SetState("Walk");
	ServerSetState("Walk"); // server
}

void APlayerCharacter::PlayFireMontage(bool bAiming)
{
	if(Combat == nullptr || Combat->EquippedWeapon == nullptr) return;

	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if(Combat->EquippedRangeWeapon)
	{
		if(AnimInstance && FireWeaponMontage)
		{
			AnimInstance->Montage_Play(FireWeaponMontage);
			FName SectionName;
			SectionName = bAiming ? FName("RifleAim") : FName("RifleHip");
			AnimInstance->Montage_JumpToSection(SectionName);
		}
	}
	else
	{
		// TODO
		if(AnimInstance && MeleeWeaponMontage)
		{
			AnimInstance->Montage_Play(MeleeWeaponMontage);
			FName SectionName;
			SectionName = FName("Attack1");
			AnimInstance->Montage_JumpToSection(SectionName);
		}
	}
	
}

void APlayerCharacter::PlayReloadMontage()
{
	if(Combat == nullptr || Combat->EquippedWeapon == nullptr) return;

	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if(AnimInstance && ReloadMontage)
	{
		AnimInstance->Montage_Play(ReloadMontage);
		FName SectionName;
		switch (Combat->EquippedWeapon->GetWeaponType())
		{
			case EWeaponType::EWT_AssaultRifle:
				SectionName = FName("AssaultRifle");
				break;
			case EWeaponType::EWT_RocketLauncher:
				SectionName = FName("AssaultRifle");
				break;
			case EWeaponType::EWT_Revolver:
				SectionName = FName("AssaultRifle");
				break;
			case EWeaponType::EWT_SMG:
				SectionName = FName("AssaultRifle");
				break;
			case EWeaponType::EWT_Shotgun:
				SectionName = FName("AssaultRifle");
				break;
			case EWeaponType::EWT_SniperRifle:
				SectionName = FName("AssaultRifle");
				break;
			case EWeaponType::EWT_MiniGun:
				SectionName = FName("AssaultRifle");
				break;
		}
		AnimInstance->Montage_JumpToSection(SectionName);
	}
}

void APlayerCharacter::PlayHitReactMontage()
{
	if(Combat == nullptr || Combat->EquippedWeapon == nullptr) return;

	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if(AnimInstance && HitReactMontage)
	{
		AnimInstance->Montage_Play(HitReactMontage);
		FName SectionName("HitFront");
		AnimInstance->Montage_JumpToSection(SectionName);
	}
}


void APlayerCharacter::ReceiveDamage(AActor* DamagedActor, float Damage, const UDamageType* DamageType,
	AController* InstigatorController, AActor* DamageCauser)
{
	Super::ReceiveDamage(DamagedActor, Damage, DamageType, InstigatorController, DamageCauser);

	// DEATH MATCH
	if(CurrentHealth == 0.f)
	{
		ADeathMatchGameMode* DeathMatchGameMode =  GetWorld()->GetAuthGameMode<ADeathMatchGameMode>();
		if(DeathMatchGameMode)
		{
			PrepperPlayerController = (PrepperPlayerController == nullptr) ? Cast<APrepperPlayerController>(Controller) : PrepperPlayerController;
			APrepperPlayerController* AttackerController = Cast<APrepperPlayerController>(InstigatorController);
			DeathMatchGameMode->PlayerEliminated(this, PrepperPlayerController, AttackerController);
		}
	}
}

void APlayerCharacter::UpdateHUDHealth()
{
	PrepperPlayerController = PrepperPlayerController == nullptr ?  Cast<APrepperPlayerController>(Controller) : PrepperPlayerController;
	if(PrepperPlayerController)
	{
		PrepperPlayerController->SetHUDHealth(CurrentHealth, MaxHealth);
	}
}

void APlayerCharacter::PollInit()
{
	if(DeathMatchPlayerState == nullptr)
	{
		DeathMatchPlayerState = GetPlayerState<ADeathMatchPlayerState>();
		if(DeathMatchPlayerState)
		{
			DeathMatchPlayerState->AddToScore(0.f);
			DeathMatchPlayerState->AddToDefeats(0);
		}
	}
}

void APlayerCharacter::Move(const FInputActionValue& Value)
{
	if(bDisableGamePlay) return;
	// input is a Vector2D
	FVector2D MovementVector = Value.Get<FVector2D>();
	
	if (Controller != nullptr)
	{
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	
		// get right vector 
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		// add movement 
		AddMovementInput(ForwardDirection, MovementVector.Y);
		AddMovementInput(RightDirection, MovementVector.X);
	}
}

void APlayerCharacter::Elim()
{
	if (Combat && Combat->EquippedWeapon)
	{
		Combat->EquippedWeapon->Dropped();
		Combat->EquippedWeapon = nullptr;
	}
	MulticastElim();
	GetWorldTimerManager().SetTimer(
		ElimTimer,
		this,
		&APlayerCharacter::ElimTimerFinished,
		ElimDelay
	);
}

void APlayerCharacter::MulticastElim()
{
	if(PrepperPlayerController)
	{
		PrepperPlayerController->SetHUDWeaponAmmo(0);
	}
	Super::MulticastElim();
	bDisableGamePlay = true;
	if(Combat)
	{
		Combat->FireButtonPressed(false);
	}
	bool bHideSniperScope = IsLocallyControlled() && 
		Combat && 
		Combat->bAiming && 
		Combat->EquippedWeapon && 
		Combat->EquippedWeapon->GetWeaponType() == EWeaponType::EWT_SniperRifle;
	if (bHideSniperScope)
	{
		ShowSniperScopeWidget(false);
	}
}

void APlayerCharacter::ElimTimerFinished()
{
	ADeathMatchGameMode* DeathMatchGameMode = GetWorld()->GetAuthGameMode<ADeathMatchGameMode>();
	if (DeathMatchGameMode)
	{
		DeathMatchGameMode->RequestRespawn(this, Controller);
	}
	if(PrepperPlayerController)
	{
		PrepperPlayerController->TargetPlayer = nullptr;
	}
}

void APlayerCharacter::Look(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// add yaw and pitch input to controller
		AddControllerYawInput(LookAxisVector.X);
		AddControllerPitchInput(LookAxisVector.Y);
	}
}

void APlayerCharacter::SpacePressed()
{
	if(bDisableGamePlay) return;
	Jump();
}

void APlayerCharacter::SpaceReleased()
{
	if(bDisableGamePlay) return;
	StopJumping();
}

void APlayerCharacter::EPressed()
{
	if(bDisableGamePlay) return;
	if(OverlappingItem)
	{
		OverlappingItem->Interaction(this);
	}
}

void APlayerCharacter::EquipWeapon(AWeapon* Weapon)
{
	UE_LOG(LogTemp, Warning, TEXT("Equip Weapon"));
	if(bDisableGamePlay) return;
	if(Combat)
	{
		Combat->EquipWeapon(Weapon);
	}
}

void APlayerCharacter::DestroyInteractionItem(AInteractable* InteractableItem)
{
	if(HasAuthority())
	{
		MulticastDestroyInteractionItem(InteractableItem);
	}
	else
	{
		ServerDestroyInteractionItem(InteractableItem);
	}
}

void APlayerCharacter::ServerDestroyInteractionItem_Implementation(AInteractable* InteractableItem)
{
	MulticastDestroyInteractionItem(InteractableItem);
}

void APlayerCharacter::MulticastDestroyInteractionItem_Implementation(AInteractable* InteractableItem)
{
	if(InteractableItem)
	{
		InteractableItem->Destroy();
	}
}


void APlayerCharacter::ControlPressed()
{
	if(bDisableGamePlay) return;
	if(bIsCrouched)
	{
		UnCrouch();
	}
	else
	{
		Crouch();
	}
}

void APlayerCharacter::RPressed()
{
	if(bDisableGamePlay) return;
	if(Combat)
	{
		Combat->Reload();
	}
}

void APlayerCharacter::MouseRightPressed()
{
	if(bDisableGamePlay) return;
	if(Combat)
	{
		Combat->SetAiming(true);
	}
}

void APlayerCharacter::MouseRightReleased()
{
	if(bDisableGamePlay) return;
	if(Combat)
	{
		Combat->SetAiming(false);
	}
}

void APlayerCharacter::MouseLeftPressed()
{
	if(bDisableGamePlay) return;
	if(Combat)
	{
		Combat->FireButtonPressed(true);
	}
}

void APlayerCharacter::MouseLeftReleased()
{
	if(bDisableGamePlay) return;
	if(Combat)
	{
		Combat->FireButtonPressed(false);
	}
}

void APlayerCharacter::CalculateAO_Pitch()
{
	AO_Pitch = GetBaseAimRotation().Pitch;
	if(AO_Pitch > 90.f && !IsLocallyControlled())
	{
		// map pitch from [ 270, 360) -> [-90,0)
		FVector2d InRange(270.f, 360.f);
		FVector2d OutRange(-90.f,0.f);
		AO_Pitch = FMath::GetMappedRangeValueClamped(InRange, OutRange, AO_Pitch);
		TurningInPlace = ETurningInPlace::ETIP_NotTurning;
	}
}

float APlayerCharacter::CalculateSpeed()
{
	FVector Velocity = GetVelocity();
	Velocity.Z = 0.f;
	return Velocity.Size();
}

void APlayerCharacter::AimOffset(float DeltaTime)
{
	if(Combat && Combat->EquippedWeapon == nullptr) return;

	float Speed = CalculateSpeed();

	bool bIsInAir = GetCharacterMovement()->IsFalling();

	if(Speed == 0.f && !bIsInAir) // Standing, not Jumping
	{
		bRotateRootBone = true;
		FRotator CurrentAimRotation = FRotator(0.f, GetBaseAimRotation().Yaw, 0.f);
		FRotator DeltaAimRotation = UKismetMathLibrary::NormalizedDeltaRotator(CurrentAimRotation, StartingAimRotation);
		AO_Yaw = DeltaAimRotation.Yaw;
		if(TurningInPlace == ETurningInPlace::ETIP_NotTurning)
		{
			InterpAO_Yaw = AO_Yaw;
		}
		bUseControllerRotationYaw = true;
		TurnInPlace(DeltaTime);
	}
	if(Speed > 0.f || bIsInAir) // Running or Jumping
	{
		bRotateRootBone = false;
		StartingAimRotation = FRotator(0.f, GetBaseAimRotation().Yaw, 0.f);
		AO_Yaw = 0.f;
		bUseControllerRotationYaw = true;
	}

	CalculateAO_Pitch();
}

void APlayerCharacter::OnRep_ReplicatedMovement()
{
	Super::OnRep_ReplicatedMovement();
	SimProxiesTurn();
	TimeSinceLastMovementReplication = 0.f;
}


void APlayerCharacter::SimProxiesTurn()
{
	if(Combat == nullptr || Combat -> EquippedWeapon == nullptr) return;

	bRotateRootBone = false;
	float Speed = CalculateSpeed();

	if(Speed > 0.f)
	{
		TurningInPlace = ETurningInPlace::ETIP_NotTurning;
		return;
	}
	
	ProxyRotationLastFrame = ProxyRotation;
	ProxyRotation = GetActorRotation();
	ProxyYaw =  UKismetMathLibrary::NormalizedDeltaRotator(ProxyRotation, ProxyRotationLastFrame).Yaw;

	if(FMath::Abs(ProxyYaw) > TurnThreshold)
	{
		if(ProxyYaw  > TurnThreshold)
		{
			TurningInPlace = ETurningInPlace::ETIP_Right;
		}
		else if (ProxyYaw < - TurnThreshold)
		{
			TurningInPlace = ETurningInPlace::ETIP_Left;
		}
		else
		{
			TurningInPlace = ETurningInPlace::ETIP_NotTurning;
		}
		return;
	}
	TurningInPlace = ETurningInPlace::ETIP_NotTurning;
}

void APlayerCharacter::Jump()
{
	if(bIsCrouched)
	{
		UnCrouch();
	}else
	{
		Super::Jump();
	}
}

void APlayerCharacter::StopJumping()
{
	Super::StopJumping();
}

void APlayerCharacter::TurnInPlace(float DeltaTime)
{
	if(AO_Yaw > 90.f)
	{
		TurningInPlace = ETurningInPlace::ETIP_Right;
	}
	else if(AO_Yaw < -90.f)
	{
		TurningInPlace = ETurningInPlace::ETIP_Left;
	}
	if(TurningInPlace != ETurningInPlace::ETIP_NotTurning)
	{
		InterpAO_Yaw = FMath::FInterpTo(InterpAO_Yaw, 0.f, DeltaTime, 10.f);
		AO_Yaw = InterpAO_Yaw;
		if(FMath::Abs(AO_Yaw) < 15.f)
		{
			TurningInPlace = ETurningInPlace::ETIP_NotTurning;
			StartingAimRotation = FRotator(0.f, GetBaseAimRotation().Yaw, 0.f);
		}
	}
}

void APlayerCharacter::ServerEquipButtonPressed_Implementation(AWeapon* Weapon)
{
}

void APlayerCharacter::SetState(const FString& state)
{
	if (state.Compare("Seat") == 0)
	{
		beforeSeat = true;
		SetActorEnableCollision(false);
		return;
	}
	if (beforeSeat)
	{
		beforeSeat = false;
		SetActorEnableCollision(true);
	}
	if (state.Compare("Aim") == 0)
	{
		// 속도 바꾸기 추가해야 함	
		return;	
	}
	if (state.Compare("Sprint") == 0)
	{
		GetCharacterMovement()->MaxWalkSpeed = SprintSpeed;
		return;
	}
	
	GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;
}

void APlayerCharacter::ServerSetState_Implementation(const FString& state)
{
	SetState(state);
}

void APlayerCharacter::Crouch(bool bClientSimulation)
{
	Super::Crouch(bClientSimulation);
	TargetSpringArmLocation = (FVector(0.0f, 0.0f, CrouchCamOffset));
	TargetArmLength = CrouchCamArmLength;
}

void APlayerCharacter::UnCrouch(bool bClientSimulation)
{
	Super::UnCrouch(bClientSimulation);

	TargetSpringArmLocation = (FVector(0.0f, 0.0f, DefaultCamOffset));
	TargetArmLength = DefaultCamArmLength;
}

void APlayerCharacter::HideCamIfCharacterClose()
{
	if (!IsLocallyControlled()) return;
	if((FollowCamera->GetComponentLocation() - GetActorLocation()).Size() < CamThreshold)
	{
		GetMesh()->SetVisibility(false);
		TArray<USceneComponent*> AttachedComponents;
		GetMesh()->GetChildrenComponents(true,  AttachedComponents);

		// Loop through all found Static Mesh components
		for (USceneComponent* SceneComponent : AttachedComponents)
		{
			UStaticMeshComponent* SMComp = Cast<UStaticMeshComponent>(SceneComponent);
			if(SMComp)
			{
				SMComp->SetVisibility(false);
			}
		}
		/*
		 * UTF-8
		 * 벽에 가까이가서 자신 캐릭터가 시야를 가릴 때
		 * 장비한 무기도 같이 가리기 위한 코드
		if(Combat && Combat->EquippedWeapon && Combat->EquippedWeapon->GetWeaponMesh())
		{
			Combat->EquippedWeapon->GetWeaponMesh()->bOwnerNoSee = true;
		}
		*/
	}
	else
	{
		GetMesh()->SetVisibility(true);
		TArray<USceneComponent*> AttachedComponents;
		GetMesh()->GetChildrenComponents(true,  AttachedComponents);

		// Loop through all found Static Mesh components
		for (USceneComponent* SceneComponent : AttachedComponents)
		{
			UStaticMeshComponent* SMComp = Cast<UStaticMeshComponent>(SceneComponent);
			if(SMComp)
			{
				SMComp->SetVisibility(true);
			}
		}
		/*
		if(Combat && Combat->EquippedWeapon && Combat->EquippedWeapon->GetWeaponMesh())
		{
			Combat->EquippedWeapon->GetWeaponMesh()->bOwnerNoSee = false;
		}
		*/
	}
}


void APlayerCharacter::Destroyed()
{
	Super::Destroyed();

	ADeathMatchGameMode* DeathMatchGameMode = Cast<ADeathMatchGameMode>(UGameplayStatics::GetGameMode(this));
	bool bMatchNotInProgress = DeathMatchGameMode && DeathMatchGameMode->GetMatchState() != MatchState::InProgress;

	if(Combat && Combat->EquippedWeapon && bMatchNotInProgress)
	{
		Combat->EquippedWeapon->Destroy();
	}
}

void APlayerCharacter::AddItem(FString ItemCode)
{
	Inven.TryAddItem(ItemCode);
}

void APlayerCharacter::SetOverlappingItem(AActor* InteractableItem)
{
	if(OverlappingItem)
    {
    	OverlappingItem->ShowPickUpWidget(false);
    }
	
    IIInteractable* TheInterface = Cast<IIInteractable>(InteractableItem);
	if (TheInterface == nullptr)
	{
		OverlappingItem = nullptr;
		return;
	}
	
	OverlappingItem = TScriptInterface<IIInteractable>(InteractableItem);
	
	if(IsLocallyControlled())
	{
		if(OverlappingItem)
		{
			OverlappingItem->ShowPickUpWidget(true);
		}
	}
	
}

void APlayerCharacter::OnRep_OverlappingItem(TScriptInterface<IIInteractable> LastItem)
{
	if (OverlappingItem)
	{
		OverlappingItem->ShowPickUpWidget(true);
	}
	if (LastItem)
	{
		LastItem->ShowPickUpWidget(false);
	}
	
}

bool APlayerCharacter::IsWeaponEquipped()
{
	return (Combat && Combat->EquippedWeapon);
}

bool APlayerCharacter::IsAiming()
{
	return (Combat && Combat->bAiming);
}

bool APlayerCharacter::IsLocallyReloading()
{
	if(Combat == nullptr) return false;
	return Combat->bLocallyReload;
}

AWeapon* APlayerCharacter::GetEquippedWeapon()
{
	if(Combat == nullptr) return nullptr;
	return Combat->EquippedWeapon;
}

FVector APlayerCharacter::GetHitTarget() const
{
	if(Combat == nullptr) return FVector();
	return Combat->HitTarget;
}

ECombatState APlayerCharacter::GetCombatState() const
{
	if (Combat == nullptr) return ECombatState::ECS_MAX;
	return Combat->CombatState;
}


