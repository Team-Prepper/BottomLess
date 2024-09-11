#include "PlayerCharacter.h"	
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "InputActionValue.h"
#include "Component/Combat/CombatComponent.h"
#include "Component/InteractionComponent.h"
#include "Component/StatusEffectComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/WidgetComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Net/UnrealNetwork.h"
#include "Prepper/Prepper.h"
#include "Prepper/GameMode/DeathMatchGameMode.h"
#include "Prepper/PlayerController/PrepperPlayerController.h"
#include "Prepper/PlayerState/DeathMatchPlayerState.h"
#include "Prepper/Weapon/WeaponActor.h"
#include "Prepper/Item/Object/ItemBackpack.h"
#include "Components/PawnNoiseEmitterComponent.h"
#include "Prepper/Component/FlexibleSpringArmComponent/FlexibleSpringArmComponent.h"

// Actor
APlayerCharacter::APlayerCharacter()
{
	PrimaryActorTick.bCanEverTick = true;
	SpawnCollisionHandlingMethod = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

	FlexibleCameraBoom = CreateDefaultSubobject<UFlexibleSpringArmComponent>(TEXT("FlexibleCameraBoom"));
	FlexibleCameraBoom->ChangeArmOffsetToTemplate(FString("Default"));
	FlexibleCameraBoom->SetupAttachment(GetMesh());
	FlexibleCameraBoom->bUsePawnControlRotation = true;

	FollowCamera = CreateDefaultSubobject<UCustomCameraComponent>(TEXT("FollowCam"));
	FollowCamera->SetupAttachment(FlexibleCameraBoom, USpringArmComponent::SocketName);
	FollowCamera->bUsePawnControlRotation = false;

	Hair = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Hair"));
	Hair->SetRelativeRotation(FRotator(-90.f, 0, 90.f));
	Hair->SetupAttachment(GetMesh(),FName("Head"));

	bUseControllerRotationYaw = false;
	GetCharacterMovement()->bOrientRotationToMovement = true;

	OverheadWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("OverheadWidget"));
	OverheadWidget->SetupAttachment(RootComponent);

	Combat = CreateDefaultSubobject<UCombatComponent>(TEXT("CombatComponent"));
	
	CombatComp = Combat;
	CombatComp->SetIsReplicated(true);
	CharacterComponents.Add(CombatComp);

	InteractionComponent = CreateDefaultSubobject<UInteractionComponent>(TEXT("InteractComponent"));
	InteractionComponent->SetIsReplicated(true);
	CharacterComponents.Add(InteractionComponent);
	
	Inventory = CreateDefaultSubobject<UMapInventory>(TEXT("Inventory"));
	Inventory->SetIsReplicated(true);

	StatusEffect = CreateDefaultSubobject<UStatusEffectComponent>(TEXT("StatusEffectComponet"));
	CharacterComponents.Add(StatusEffect);
	
	GetCharacterMovement()->NavAgentProps.bCanCrouch = true;
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);

	GetMesh()->SetCollisionObjectType(ECC_SkeletalMesh);
	GetMesh()->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);
	GetMesh()->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
	GetCharacterMovement()->RotationRate = FRotator(0.f, 0.f, 850.f);

	TurningInPlace = ETurningInPlace::ETIP_NotTurning;

	NetUpdateFrequency = 66.f;
	MinNetUpdateFrequency = 33.f;

	bBeforeSeat = false;

	// 노이즈 생성 컴포넌트 추가
	PawnNoiseEmitter = CreateDefaultSubobject<UPawnNoiseEmitterComponent>(TEXT("PawnNoiseEmitter"));
}

void APlayerCharacter::Tick(float DeltaTime)
{
	ACharacter::Tick(DeltaTime);

	RotateInPlace(DeltaTime);
	HideCamIfCharacterClose();
	
	PollInit();
	if (MovementState == EMovementState::EMS_Sprint)
	{
		MakeNoise(1, nullptr, FVector::ZeroVector);
	}
}

void APlayerCharacter::Destroyed()
{
	Super::Destroyed();

	ADeathMatchGameMode* DeathMatchGameMode = Cast<ADeathMatchGameMode>(UGameplayStatics::GetGameMode(this));
	bool bMatchNotInProgress = DeathMatchGameMode && DeathMatchGameMode->GetMatchState() != MatchState::InProgress;

	if(CombatComp && CombatComp->EquippedWeapon && bMatchNotInProgress)
	{
		//CombatComp->EquippedWeapon->Destroy();
	}
}

void APlayerCharacter::BeginPlay()
{
	Super::BeginPlay();
}

// BaseCharacter

void APlayerCharacter::Crouch(bool bClientSimulation)
{
	Super::Crouch(bClientSimulation);

	FlexibleCameraBoom->ChangeArmOffsetToTemplate(FString("Crouch"));
}

void APlayerCharacter::UnCrouch(bool bClientSimulation)
{
	Super::UnCrouch(bClientSimulation);

	FlexibleCameraBoom->ChangeArmOffsetToTemplate(FString("Default"));
}

void APlayerCharacter::Elim()
{
	
	APrepperPlayerController* PrepperPlayerController = Cast<APrepperPlayerController>(Controller);
	PrepperPlayerController->ResetPlayer();

	const bool bHideSniperScope = IsLocallyControlled() && 
		CombatComp && 
		CombatComp->bAiming && 
		CombatComp->EquippedWeapon && 
		CombatComp->EquippedWeapon->GetWeaponType() == EWeaponType::EWT_SniperRifle;
	
	if (bHideSniperScope)
	{
		ShowSniperScopeWidget(false);
	}

	Super::Elim();
	
}


void APlayerCharacter::MulticastElim()
{
	Super::MulticastElim();
	bDisableGamePlay = true;
}

void APlayerCharacter::ReceiveDamage(float Damage, AController* InstigatorController, AActor* DamageCauser)
{
	Super::ReceiveDamage(Damage, InstigatorController, DamageCauser);
	Notify();
}

void APlayerCharacter::PlayHitReactMontage()
{
	if(CombatComp == nullptr || CombatComp->EquippedWeapon == nullptr) return;
	ABaseCharacter::PlayHitReactMontage();
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

// IPlayerAbility

void APlayerCharacter::AddItem(const FString& ItemCode)
{
	if(!HasAuthority()) return;
	MulticastAddItem(ItemCode);
}

void APlayerCharacter::UseQuickSlotItem(int Idx)
{
	Inventory->UseItemAtQuickSlot(Idx);
}

void APlayerCharacter::EquipWeapon(AWeaponActor* Weapon)
{
	UE_LOG(LogTemp, Warning, TEXT("Equip Weapon"));
	if(bDisableGamePlay) return;
	if(CombatComp)
	{
		CombatComp->EquipWeapon(Weapon);
	}
}

void APlayerCharacter::EquipBackpack(AItemBackpack* BackpackToEquip)
{
	if(BackpackToEquip == nullptr) return;

	EquippedBackpack = BackpackToEquip;
	EquippedBackpack->SetBackpackState(EBackpackState::EBS_Equipped);

	if(IsLocallyControlled())
	{
		if(GetController())
			PrepperHUD = PrepperHUD == nullptr ? Cast<APrepperHUD>(Cast<APlayerController>(GetController())->GetHUD()) : PrepperHUD;
		else
			UE_LOG(LogTemp, Warning, TEXT("!!!!! SOMETHING TERRIBLE ERROR !!!!! : NO CONTORLLER"));
	
	
		if (!PrepperHUD || !PrepperHUD->CharacterOverlay)
		{
			UE_LOG(LogTemp, Warning, TEXT("INVENTORY OBSERVER : NO PREPPER HUD"));
			return;
		}
		EquippedBackpack->GetInventory()->Attach(PrepperHUD->CharacterOverlay);
	}
}

void APlayerCharacter::Heal(float Amount)
{
	UE_LOG(LogTemp, Warning, TEXT("Heal:%f"), Amount);
	CurrentHealth += Amount;
	if (CurrentHealth > MaxHealth) CurrentHealth = MaxHealth;
	Notify();
}

void APlayerCharacter::Eat(float Amount)
{
	UE_LOG(LogTemp, Warning, TEXT("Eat:%f"), Amount);
}

void APlayerCharacter::Drink(float Amount)
{
	UE_LOG(LogTemp, Warning, TEXT("Drink:%f"), Amount);
}

void APlayerCharacter::OnRep_EquippedBackpack()
{
	if(EquippedBackpack)
	{
		EquipBackpack(EquippedBackpack);
	}
}

void APlayerCharacter::MulticastAddItem_Implementation(const FString& ItemCode)
{
	if(EquippedBackpack)
	{
		UE_LOG(LogTemp, Warning, TEXT("Backpack : Add Item %s"), *ItemCode);
		if(EquippedBackpack->GetInventory()->TryAddItem(ItemCode))
			return;
	}
	UE_LOG(LogTemp, Warning, TEXT("pocket : Add Item %s"), *ItemCode);
	Inventory->TryAddItem(ItemCode);
}

// IControllable

void APlayerCharacter::Move(const FInputActionValue& Value)
{
	if(bDisableGamePlay) return;
	// input is a Vector2D
	FVector2D MovementVector = Value.Get<FVector2D>();
	
	if (Controller == nullptr) return;
	
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

void APlayerCharacter::ShiftPressed()
{
	if(bDisableGamePlay) return;
	if(StatusEffect && StatusEffect->StatusFlags.HasEffect(EStatusEffect::ESE_THIRSTY))
	{
		UE_LOG(LogTemp, Warning, TEXT("THIRSTY : Can't Sprint"));
		SetMovementState(EMovementState::EMS_Idle);
		return;
	}
	if(MovementState == EMovementState::EMS_Sprint) return;
	
	SetMovementState(EMovementState::EMS_Sprint);
}

void APlayerCharacter::ShiftReleased()
{
	if(bDisableGamePlay) return;
	SetMovementState(EMovementState::EMS_Idle);
}

void APlayerCharacter::EPressed()
{
	if(bDisableGamePlay) return;

	/* Interaction */
	if(InteractionComponent && InteractionComponent->CurInteractableItem)
	{
		UE_LOG(LogTemp, Warning , TEXT("INTERACTABLE ITEM"));
		InteractionComponent->CurInteractableItem->Interaction(this);
		return;
	}

	/* Swap */
	if (CombatComp)
	{
		UE_LOG(LogTemp, Warning , TEXT("COMBAT COMP : SWAP"));
		CombatComp->EquipWeapon(nullptr);
	}

}

void APlayerCharacter::RPressed()
{
	if(bDisableGamePlay) return;
	Reload();
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

void APlayerCharacter::MouseLeftPressed()
{
	if(bDisableGamePlay) return;
	AttackTrigger(true);
}

void APlayerCharacter::MouseLeftReleased()
{
	if(bDisableGamePlay) return;
	AttackTrigger(false);
}

void APlayerCharacter::MouseRightPressed()
{
	if(bDisableGamePlay) return;
	AimTrigger(true);
}

void APlayerCharacter::MouseRightReleased()
{
	if(bDisableGamePlay) return;
	AimTrigger(false);
}

void APlayerCharacter::ToggleInventory()
{ 
	if(!EquippedBackpack) return;
	
	UE_LOG(LogTemp,Warning,TEXT("InvenToggle"));
	EquippedBackpack->OpenInventory();
	
	MulticastToggleInventory();
}

void APlayerCharacter::SeatToggle(const bool Seat)
{
	SetActorEnableCollision(!Seat);
	SetActorHiddenInGame(Seat);
	SetPlayerEquipmentHiddenInGame(Seat);
}

void APlayerCharacter::MulticastToggleInventory_Implementation()
{
	if(IsLocallyControlled())
	{
		APlayerController* PlayerController = Cast<APlayerController>(GetController());
		PrepperHUD = PrepperHUD == nullptr ? Cast<APrepperHUD>(PlayerController->GetHUD()) : PrepperHUD;
	
		if (PrepperHUD && PrepperHUD->CharacterOverlay)
		{
			UE_LOG(LogTemp, Warning, TEXT("Complete Detach inventory Observer"));
			EquippedBackpack->GetInventory()->Detach(PrepperHUD->CharacterOverlay);
			PrepperHUD->CharacterOverlay->ClearAllItemIcons();
		}else
		{
			UE_LOG(LogTemp, Warning, TEXT("INVENTORY OBSERVER : NO PREPPER HUD"));
		}
	}
	
	EquippedBackpack = nullptr;
}

UCustomCameraComponent* APlayerCharacter::GetFollowCamera()
{
	return FollowCamera;
}

void APlayerCharacter::CalculateAO_Pitch()
{
	AO_Pitch = GetBaseAimRotation().Pitch;
	if(AO_Pitch > 90.f && !IsLocallyControlled())
	{
		// map pitch from [ 270, 360) -> [-90,0)
		const FVector2d InRange(270.f, 360.f);
		const FVector2d OutRange(-90.f,0.f);
		AO_Pitch = FMath::GetMappedRangeValueClamped(InRange, OutRange, AO_Pitch);
		TurningInPlace = ETurningInPlace::ETIP_NotTurning;
	}
}

void APlayerCharacter::AimOffset(float DeltaTime)
{
	if (CombatComp && CombatComp->EquippedWeapon == nullptr) return;

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

void APlayerCharacter::SimProxiesTurn()
{
	if(CombatComp == nullptr || CombatComp-> EquippedWeapon == nullptr) return;

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
		return;
	}
	
	TimeSinceLastMovementReplication += DeltaTime;
	if (TimeSinceLastMovementReplication > 0.25f)
	{
		OnRep_ReplicatedMovement();
	}
	CalculateAO_Pitch();
}

void APlayerCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	DOREPLIFETIME(APlayerCharacter, bDisableGamePlay);
	DOREPLIFETIME(APlayerCharacter, MovementState);
	DOREPLIFETIME(APlayerCharacter, EquippedBackpack);
}

// ETC
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

void APlayerCharacter::OnRep_ReplicatedMovement()
{
	Super::OnRep_ReplicatedMovement();
	SimProxiesTurn();
	TimeSinceLastMovementReplication = 0.f;
}

void APlayerCharacter::SetPlayerEquipmentHiddenInGame(bool visible)
{
	SetAttachedHidden(visible);
}

void APlayerCharacter::SetEquipmentHidden(AActor* Target, bool visible)
{
	if (!Target) return;
	Target->SetActorHiddenInGame(visible);
}

void APlayerCharacter::HideCamIfCharacterClose()
{
	if (!IsLocallyControlled()) return;
	if((FollowCamera->GetComponentLocation() - GetActorLocation()).Size() < CamThreshold)
	{
		SetActorHiddenInGame(true);
	}
	else
	{
		SetActorHiddenInGame(false);
	}
}

float APlayerCharacter::CalculateSpeed()
{
	FVector Velocity = GetVelocity();
	Velocity.Z = 0.f;
	return Velocity.Size();
}

bool APlayerCharacter::IsWeaponEquipped()
{
	return (CombatComp && CombatComp->EquippedWeapon);
}

bool APlayerCharacter::IsAiming()
{
	return GetMovementState() == EMovementState::EMS_Aim;
}

bool APlayerCharacter::IsLocallyReloading()
{
	if(CombatComp == nullptr) return false;
	return CombatComp->bLocallyReload;
}

AWeaponActor* APlayerCharacter::GetEquippedWeapon()
{
	if(CombatComp == nullptr) return nullptr;
	return CombatComp->EquippedWeapon;
}

FVector APlayerCharacter::GetHitTarget() const
{
	if(CombatComp == nullptr) return FVector();
	return CombatComp->HitTarget;
}

ECombatState APlayerCharacter::GetCombatState() const
{
	if(CombatComp == nullptr) return ECombatState::ECS_MAX;
	return CombatComp->CombatState;
}