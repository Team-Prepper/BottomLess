#include "CombatComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"
#include "Prepper/Character/PlayerCharacter.h"
#include "Prepper/PlayerController/PrepperPlayerController.h"
#include "Prepper/Weapon/MeleeWeapon.h"
#include "Prepper/Weapon/WeaponActor.h"
#include "Prepper/Weapon/RangeWeapon/RangeWeapon.h"

UCombatComponent::UCombatComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UCombatComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UCombatComponent, EquippedWeapon);
	DOREPLIFETIME(UCombatComponent, SecondaryWeapon);
	DOREPLIFETIME(UCombatComponent, EquippedRangeWeapon);
	DOREPLIFETIME(UCombatComponent, EquippedMeleeWeapon);
	DOREPLIFETIME(UCombatComponent, bAiming);
	DOREPLIFETIME_CONDITION(UCombatComponent, CarriedAmmo, COND_OwnerOnly);
	DOREPLIFETIME(UCombatComponent, CombatState);
}

void UCombatComponent::BeginPlay()
{
	Super::BeginPlay();

	if (!Character) return;
	
	UE_LOG(LogTemp, Warning, TEXT("CombatComponentReady"));
	Character->SetPlayerMovementState(EPlayerMovementState::EPMS_Idle);

	if (Character->GetFollowCamera())
	{
		DefaultFOV = Character->GetFollowCamera()->FieldOfView;
		CurrentFOV = DefaultFOV;
	}
	if (Character->HasAuthority())
	{
		InitCarriedAmmo();
	}
}

void UCombatComponent::TickComponent(float DeltaTime, ELevelTick TickType,
                                     FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (!Character) return;
	if (!Character->IsLocallyControlled()) return;
	
	// 총구의 방향을 내 화면의 방향과 일치 시키기 위해서 틱에서 처리 
	FHitResult HitResult;
	TraceUnderCrosshair(HitResult);
	if (HitResult.bBlockingHit)
	{
		HitTarget = HitResult.ImpactPoint;
	}
	else
	{
		HitTarget = HitResult.Location;
	}

	SetHUDCrosshair(DeltaTime);
	InterpFOV(DeltaTime);
}

void UCombatComponent::SetHUDCrosshair(float DeltaTime)
{
	if (Character == nullptr || Character->Controller == nullptr) return;

	Controller = Controller == nullptr ? Cast<APrepperPlayerController>(Character->Controller) : Controller;
	if (!Controller) return;
	
	HUD = HUD == nullptr ? Cast<APrepperHUD>(Controller->GetHUD()) : HUD;
	if (!HUD) return;
	if (!EquippedWeapon) return;
	
	EquippedWeapon->GetCrosshair(DeltaTime, bAiming,
								HUDPackage.CrosshairCenter,
								HUDPackage.CrosshairLeft,
								HUDPackage.CrosshairRight,
								HUDPackage.CrosshairTop,
								HUDPackage.CrosshairBottom,
								HUDPackage.CrosshairSpread);

	HUD->SetHUDPackage(HUDPackage);
}


void UCombatComponent::InterpFOV(float DeltaTime)
{
	if (EquippedRangeWeapon == nullptr) return;
	if (Character == nullptr) return;
	if (Character->GetFollowCamera() == nullptr) return;

	if (bAiming)
	{
		CurrentFOV = FMath::FInterpTo(CurrentFOV, EquippedRangeWeapon->GetZoomedFOV(), DeltaTime,
		                              EquippedRangeWeapon->GetZoomedInterpSpeed());
	}
	else
	{
		CurrentFOV = FMath::FInterpTo(CurrentFOV, DefaultFOV, DeltaTime, ZoomInterpSpeed);
	}

	Character->GetFollowCamera()->SetFieldOfView(CurrentFOV);
}

// TODO
// Fire Start
void UCombatComponent::FireButtonPressed(bool bPressed)
{
	if (EquippedRangeWeapon && EquippedRangeWeapon->bAutomatic == false)
	{
		if (bFireButtonPressed == bPressed) return;
	}
	bFireButtonPressed = bPressed;

	if (bFireButtonPressed)
	{
		Fire();
	}
}


// TODO
bool UCombatComponent::CanFire()
{
	if (!EquippedWeapon) return false;
	if (CombatState != ECombatState::ECS_Unoccupied) return false;

	if (EquippedMeleeWeapon) return true;
	return !EquippedRangeWeapon->IsAmmoEmpty();
		
}

void UCombatComponent::Fire()
{
	if (!CanFire()) return;

	if (!EquippedWeapon) return;
		
	CrosshairShootingFactor = .75f;
	HitTargets = EquippedWeapon->GetTarget(HitTarget);
	LocalFireWeapon(HitTargets);
	ServerFireWeapon(HitTargets);

	CombatState = ECombatState::ECS_Fire;
	StartFireTimer();
}

void UCombatComponent::LocalFireWeapon(const TArray<FVector_NetQuantize>& TraceHitTargets)
{

	if (EquippedWeapon == nullptr || Character == nullptr) return;
	
	if (CombatState != ECombatState::ECS_Unoccupied) return;
	
	EquippedWeapon->Fire(TraceHitTargets);	

	UAnimInstance* AnimInstance = Character->GetMesh()->GetAnimInstance();
	if (!AnimInstance) return;
	
	if (EquippedRangeWeapon)
	{
		if(!FireWeaponMontage) return;
		
		AnimInstance->Montage_Play(FireWeaponMontage);
		FName SectionName = bAiming ? FName("FireAim") : FName("FireHip");
		AnimInstance->Montage_JumpToSection(SectionName);
		
	}
	else
	{
		if(!MeleeWeaponMontage) return;
	
		AnimInstance->Montage_Play(MeleeWeaponMontage);
		FName SectionName = EquippedMeleeWeapon->GetWeaponType() == EWeaponType::EWT_MeleeWeaponBlunt ? FName("Attack1") : FName("Attack2");
		AnimInstance->Montage_JumpToSection(SectionName);
	}
	
}


void UCombatComponent::ServerFireWeapon_Implementation(const TArray<FVector_NetQuantize>& TraceHitTargets)
{
	// 노이즈 발생
	EquippedWeapon->MakeNoise(1, Character, FVector::ZeroVector);
	MulticastFireWeapon(TraceHitTargets);
}

void UCombatComponent::MulticastFireWeapon_Implementation(const TArray<FVector_NetQuantize>& TraceHitTargets)
{
	if (Character && Character->IsLocallyControlled()) return;
	LocalFireWeapon(TraceHitTargets);
}


void UCombatComponent::StartFireTimer()
{
	if (EquippedWeapon == nullptr || Character == nullptr) return;
	Character->GetWorldTimerManager().SetTimer(
		FireTimer,
		this,
		&UCombatComponent::FireTimerFinished,
		EquippedWeapon->FireDelay
	);
}

void UCombatComponent::FireTimerFinished()
{
	if (EquippedWeapon == nullptr) return;

	CombatState = ECombatState::ECS_Unoccupied;
	
	if (!EquippedRangeWeapon) return;
	if (bFireButtonPressed && EquippedRangeWeapon->bAutomatic)
	{
		Fire();
	}
	if (EquippedRangeWeapon->GetAutoReload())
	{
		ReloadEmptyWeapon();
	}
}

void UCombatComponent::ReloadEmptyWeapon()
{
	if (EquippedRangeWeapon && EquippedRangeWeapon->IsAmmoEmpty())
	{
		Reload();
	}
}


void UCombatComponent::OnRep_CarriedAmmo()
{
	Controller = Controller == nullptr ? Cast<APrepperPlayerController>(Character->Controller) : Controller;
	if (Controller)
	{
		Controller->SetHUDCarriedAmmo(CarriedAmmo);
	}
}

void UCombatComponent::InitCarriedAmmo()
{
	CarriedAmmoMap.Emplace(EWeaponType::EWT_AssaultRifle, StartingARAmmo);
	CarriedAmmoMap.Emplace(EWeaponType::EWT_RocketLauncher, StartingRocketAmmo);
	CarriedAmmoMap.Emplace(EWeaponType::EWT_Revolver, StartingRevolverAmmo);
	CarriedAmmoMap.Emplace(EWeaponType::EWT_SMG, StartingSMGAmmo);
	CarriedAmmoMap.Emplace(EWeaponType::EWT_Shotgun, StartingShotgunAmmo);
	CarriedAmmoMap.Emplace(EWeaponType::EWT_SniperRifle, StartingSniperRifleAmmo);
	CarriedAmmoMap.Emplace(EWeaponType::EWT_MiniGun, StartingMiniGunAmmo);
}

void UCombatComponent::PickupAmmo(EWeaponType WeaponType, int32 AmmoAmount)
{
	if (CarriedAmmoMap.Contains(WeaponType))
	{
		CarriedAmmoMap[WeaponType] = FMath::Clamp(CarriedAmmoMap[WeaponType] + AmmoAmount, 0, MaxCarriedAmmo);
		UpdateCarriedAmmo();
	}
	if (EquippedRangeWeapon && EquippedRangeWeapon->IsAmmoEmpty() && EquippedWeapon->GetWeaponType() == WeaponType)
	{
		Reload();
	}
}

EWeaponType UCombatComponent::SetWeaponType()
{
	if (EquippedWeapon)
	{
		EquippedRangeWeapon = Cast<ARangeWeapon>(EquippedWeapon);
		EquippedMeleeWeapon = Cast<AMeleeWeapon>(EquippedWeapon);
		return EquippedWeapon->GetWeaponType();
	}
	return EWeaponType::EWT_MAX;
}


void UCombatComponent::OnRep_Aiming()
{
	if (Character && Character->IsLocallyControlled())
	{
		bAiming = bAimButtonPressed;
	}
}

void UCombatComponent::SetAiming(bool bIsAiming)
{
	if (Character == nullptr || EquippedWeapon == nullptr) return;
	if (bAiming == bIsAiming) return;

	bAiming = bIsAiming;
	ServerSetAiming(bIsAiming);
	if (Character && Character->IsLocallyControlled())
	{
		EPlayerMovementState NewState = bIsAiming ? EPlayerMovementState::EPMS_Aim : EPlayerMovementState::EPMS_Idle;
		Character->SetPlayerMovementState(NewState);
		bAimButtonPressed = bIsAiming;
		if (EquippedWeapon->GetWeaponType() == EWeaponType::EWT_SniperRifle)
		{
			Character->ShowSniperScopeWidget(bIsAiming);
		}
	}
}

void UCombatComponent::ServerSetAiming_Implementation(bool bIsAiming)
{
	bAiming = bIsAiming;
	if (Character)
	{
		EPlayerMovementState NewState = bIsAiming ? EPlayerMovementState::EPMS_Aim : EPlayerMovementState::EPMS_Idle;
		Character->SetPlayerMovementState(NewState);
	}
}


void UCombatComponent::EquipWeapon(AWeaponActor* WeaponToEquip)
{
	if (Character == nullptr || WeaponToEquip == nullptr) return;
	if (CombatState != ECombatState::ECS_Unoccupied) return;
	WeaponToEquip->SetActorEnableCollision(false);

	if (EquippedWeapon != nullptr && SecondaryWeapon == nullptr)
	{
		EquipSecondaryWeapon(WeaponToEquip);
	}
	else
	{
		EquipPrimaryWeapon(WeaponToEquip);
	}

	Character->GetCharacterMovement()->bOrientRotationToMovement = false;
	Character->bUseControllerRotationYaw = true;
}

void UCombatComponent::OnRep_EquippedWeapon()
{
	if (!EquippedWeapon || !Character) return;

	EquippedWeapon->SetWeaponState(EWeaponState::EWS_Equipped);

	Character->AttachActorAtSocket(EquippedWeapon->AttachSocketName(), EquippedWeapon);

	Character->GetCharacterMovement()->bOrientRotationToMovement = false;
	Character->bUseControllerRotationYaw = true;
}

void UCombatComponent::OnRep_SecondaryWeapon()
{
	if (SecondaryWeapon && Character)
	{
		SecondaryWeapon->SetWeaponState(EWeaponState::EWS_Holstered);
	}
}

void UCombatComponent::EquipPrimaryWeapon(AWeaponActor* WeaponToEquip)
{
	if (WeaponToEquip == nullptr) return;
	DropEquippedWeapon();
	EquippedWeapon = WeaponToEquip;
	EquippedWeapon->SetOwner(Character);
	EquippedWeapon->SetWeaponState(EWeaponState::EWS_Equipped);
	SetWeaponType();
	EquippedWeapon->SetHUDAmmo();
	UpdateCarriedAmmo();
	ReloadEmptyWeapon();
}

void UCombatComponent::EquipSecondaryWeapon(AWeaponActor* WeaponToEquip)
{
	if (WeaponToEquip == nullptr) return;
	SecondaryWeapon = WeaponToEquip;
	SecondaryWeapon->SetOwner(Character);
	SecondaryWeapon->SetWeaponState(EWeaponState::EWS_Holstered);
}

void UCombatComponent::DropEquippedWeapon()
{
	if (!EquippedWeapon) return;
	
	EquippedWeapon->SetWeaponState(EWeaponState::EWS_Dropped);
	EquippedMeleeWeapon = nullptr;
	EquippedRangeWeapon = nullptr;
}

void UCombatComponent::SwapWeapons()
{
	if (CombatState != ECombatState::ECS_Unoccupied || Character == nullptr || !Character->HasAuthority()) return;
	MulticastSwapWeapon();
}

void UCombatComponent::MulticastSwapWeapon_Implementation()
{
	CombatState = ECombatState::ECS_SwappingWeapons;

	FinishSwapAttachWeapons();
	GetWorld()->GetTimerManager().SetTimer(SwapDelayTimer, this, &UCombatComponent::FinishSwap, 1.5f, false);
	
	UAnimInstance* AnimInstance = Character->GetMesh()->GetAnimInstance();
	
	if (!AnimInstance || !SwapMontage) return;
	AnimInstance->Montage_Play(SwapMontage);
	
}

void UCombatComponent::FinishSwap()
{
	if (Character)
	{
		CombatState = ECombatState::ECS_Unoccupied;
	}
}

void UCombatComponent::FinishSwapAttachWeapons()
{

	if (Character == nullptr) return;
	AWeaponActor* TempWeapon = EquippedWeapon;
	EquippedWeapon = SecondaryWeapon;
	SecondaryWeapon = TempWeapon;

	EquippedWeapon->SetWeaponState(EWeaponState::EWS_Equipped);
	SetWeaponType();
	EquippedWeapon->SetHUDAmmo();
	UpdateCarriedAmmo();
	ReloadEmptyWeapon();

	SecondaryWeapon->SetWeaponState(EWeaponState::EWS_Holstered);
}

void UCombatComponent::UpdateCarriedAmmo()
{
	if (EquippedWeapon == nullptr) return;
	if (CarriedAmmoMap.Contains(EquippedWeapon->GetWeaponType()))
	{
		CarriedAmmo = CarriedAmmoMap[EquippedWeapon->GetWeaponType()];
	}
	else
	{
		CarriedAmmo = -1;
	}

	Controller = Controller == nullptr ? Cast<APrepperPlayerController>(Character->Controller) : Controller;
	if (Controller)
	{
		Controller->SetHUDCarriedAmmo(CarriedAmmo);
	}
}

void UCombatComponent::Reload()
{
	if (CarriedAmmo > 0 &&
		CombatState != ECombatState::ECS_Reloading &&
		EquippedRangeWeapon &&
		!EquippedRangeWeapon->IsAmmoFull())
	{
		ServerReload();
		HandleReload();
	}
}

void UCombatComponent::ServerReload_Implementation()
{
	if (Character == nullptr || EquippedWeapon == nullptr) return;
	if (!Character->IsLocallyControlled())
	{
		CombatState = ECombatState::ECS_Reloading;
		HandleReload();
	}
}

void UCombatComponent::FinishReloading()
{
	if (Character == nullptr) return;
	CombatState = ECombatState::ECS_Unoccupied;
	if (Character->HasAuthority())
	{
		UpdateAmmoValues();
	}
	if (bFireButtonPressed)
	{
		Fire();
	}
}

void UCombatComponent::UpdateAmmoValues()
{
	if (Character == nullptr || EquippedRangeWeapon == nullptr) return;
	int32 ReloadAmount = AmountToReload();
	if (CarriedAmmoMap.Contains(EquippedWeapon->GetWeaponType()))
	{
		CarriedAmmoMap[EquippedWeapon->GetWeaponType()] -= ReloadAmount;
		CarriedAmmo = CarriedAmmoMap[EquippedWeapon->GetWeaponType()];
	}
	Controller = Controller == nullptr ? Cast<APrepperPlayerController>(Character->Controller) : Controller;
	if (Controller)
	{
		Controller->SetHUDCarriedAmmo(CarriedAmmo);
	}
	EquippedRangeWeapon->AddAmmo(ReloadAmount);
}

int32 UCombatComponent::AmountToReload()
{
	if (EquippedRangeWeapon == nullptr) return 0;
	int32 RoomInMag = EquippedRangeWeapon->GetMagCapacity() - EquippedRangeWeapon->GetAmmo();

	if (CarriedAmmoMap.Contains(EquippedWeapon->GetWeaponType()))
	{
		int32 AmountCarried = CarriedAmmoMap[EquippedWeapon->GetWeaponType()];
		int32 Least = FMath::Min(RoomInMag, AmountCarried);
		return FMath::Clamp(RoomInMag, 0, Least);
	}
	return 0;
}

void UCombatComponent::OnRep_CombatState()
{
	switch (CombatState)
	{
	case ECombatState::ECS_Reloading:
		if (Character && !Character->IsLocallyControlled())
		{
			HandleReload();
		}
		break;
	case ECombatState::ECS_Unoccupied:
		if (bFireButtonPressed)
		{
			Fire();
		}
		break;
	}
}

void UCombatComponent::HandleReload()
{
	if (!Character) return;
	
	UAnimInstance* AnimInstance = Character->GetMesh()->GetAnimInstance();
	
	if(!AnimInstance || !ReloadMontage) return;
	
	AnimInstance->Montage_Play(ReloadMontage);
	AnimInstance->Montage_JumpToSection(EquippedWeapon->ReloadActionName);
	
}


void UCombatComponent::TraceUnderCrosshair(FHitResult& TraceHitResult)
{
	FVector2D ViewportSize;
	if (GEngine && GEngine->GameViewport)
	{
		GEngine->GameViewport->GetViewportSize(ViewportSize);
	}

	const FVector2D CrosshairLocation(ViewportSize.X / 2.f, ViewportSize.Y / 2.f);
	FVector CrosshairWorldPosition;
	FVector CrosshairWorldDirection;
	const bool bScreenToWorld = UGameplayStatics::DeprojectScreenToWorld(
		UGameplayStatics::GetPlayerController(this, 0),
		CrosshairLocation,
		CrosshairWorldPosition,
		CrosshairWorldDirection
	);

	if (!bScreenToWorld) return;
	
	FVector Start = CrosshairWorldPosition;

	if (Character)
	{
		const float DistanceToCharacter = (Character->GetActorLocation() - Start).Size();
		Start += CrosshairWorldDirection * (DistanceToCharacter + 100.f);
	}
	
	const FVector End = Start + CrosshairWorldDirection * TRACE_LEN;
	const ECollisionChannel CollisionChannel = ECC_Visibility;

	const bool bHitSomething = GetWorld()->LineTraceSingleByChannel(
		TraceHitResult, Start, End, CollisionChannel);
	
	if (!bHitSomething)
	{
		// TraceHitResult의 Location에 End 좌표 설정
		TraceHitResult.Location = End;
	}

	if (TraceHitResult.GetActor() && TraceHitResult.GetActor()->Implements<UInteractWithCrosshairInterface>())
	{
		HUDPackage.CrosshairColor = FLinearColor::Red;
	}
	else
	{
		HUDPackage.CrosshairColor = FLinearColor::White;
	}
}

bool UCombatComponent::ShouldSwapWeapons()
{
	return (EquippedWeapon != nullptr && SecondaryWeapon != nullptr);
}
