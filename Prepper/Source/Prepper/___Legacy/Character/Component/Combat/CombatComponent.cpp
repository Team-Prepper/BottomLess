#include "CombatComponent.h"

#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"
#include "Prepper/___Legacy/Character/PlayerCharacter.h"
#include "Prepper/___Legacy/HUD/PrepperHUD.h"
#include "Prepper/___Legacy/Weapon/RangeWeapon/RangeWeapon.h"
#include "Prepper/__Base/Util/GaugeInt.h"

// Actor
UCombatComponent::UCombatComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
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

	FLinearColor CrosshairColor;
	if (HitResult.GetActor() &&
		HitResult.GetActor()->Implements<UInteractWithCrosshairInterface>())
	{
		CrosshairColor = FLinearColor::Red;
	}
	else
	{
		CrosshairColor = FLinearColor::White;
	}
	
	if (HitResult.bBlockingHit)
	{
		HitTarget = HitResult.ImpactPoint;
	}
	else
	{
		HitTarget = HitResult.Location;
	}

	SetHUDCrosshair(DeltaTime, CrosshairColor);
}

void UCombatComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UCombatComponent, SecondaryWeapon);
	DOREPLIFETIME_CONDITION(UCombatComponent, CarriedAmmo, COND_OwnerOnly);
	DOREPLIFETIME(UCombatComponent, ReplicatedWeaponAmmoData);
}

void UCombatComponent::BeginPlay()
{
	Super::BeginPlay();

	if (!Character) return;
	
	UE_LOG(LogTemp, Warning, TEXT("CombatComponentReady"));
}

// Equipped Weapon

void UCombatComponent::ActionEnd()
{
	if (EquippedRangeWeapon == nullptr) return;
	
	if (EquippedRangeWeapon->bAutoReload)
	{
		ReloadEmptyWeapon();
	}
	if (EquippedRangeWeapon->bAutomatic)
	{
		Fire();
	}
	
}

void UCombatComponent::EquipWeapon(AWeapon* WeaponToEquip)
{
	if (WeaponToEquip == nullptr)
	{
		SwapWeapons();
		return;
	}
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
		NotifyWeapon();
	}

	Character->GetCharacterMovement()->bOrientRotationToMovement = false;
	Character->bUseControllerRotationYaw = true;
}

void UCombatComponent::EquipPrimaryWeapon(AWeapon* WeaponToEquip)
{
	if (WeaponToEquip == nullptr) return;
	DropEquippedWeapon();
	EquippedWeapon = WeaponToEquip;
	
	EquippedWeapon->OnEquipped(Character);
	EquippedWeapon->SetWeaponHandler(this);
	SetWeaponType();
	UpdateCarriedAmmo();
	ReloadEmptyWeapon();
}

void UCombatComponent::EquipSecondaryWeapon(AWeapon* WeaponToEquip)
{
	if (WeaponToEquip == nullptr) return;
	
	SecondaryWeapon = WeaponToEquip;
	SecondaryWeapon->OnEquippedSecondary(Character);
	SecondaryWeapon->SetWeaponHandler(this);
}

void UCombatComponent::DropEquippedWeapon()
{
	if (!EquippedWeapon) return;
	
	EquippedWeapon->OnDropped(Character);
	EquippedRangeWeapon = nullptr;
	
	if (!Character->IsLocallyControlled()) return;

	EquippedWeapon->SetStateUnAiming();
}

void UCombatComponent::SetWeaponType()
{
	if (!EquippedWeapon) return;
	EquippedRangeWeapon = Cast<ARangeWeapon>(EquippedWeapon);
}

void UCombatComponent::OnRep_EquippedWeapon()
{
	Super::OnRep_EquippedWeapon();
	NotifyAmmo();
	NotifyWeapon();
}

void UCombatComponent::OnRep_SecondaryWeapon()
{
	if (!EquippedWeapon) return;
	if (!Character) return;
	
	SecondaryWeapon->OnEquippedSecondary(Character);
	SecondaryWeapon->SetWeaponHandler(this);
}

// Swap Weapon

void UCombatComponent::SwapWeapons()
{
	if (CombatState != ECombatState::ECS_Unoccupied || Character == nullptr || !Character->HasAuthority()) return;
	if (EquippedWeapon == nullptr || SecondaryWeapon == nullptr) return;
	MulticastSwapWeapon();
}
void UCombatComponent::MulticastSwapWeapon_Implementation()
{
	CombatState = ECombatState::ECS_SwappingWeapons;

	FinishSwapAttachWeapons();
	GetWorld()->GetTimerManager().SetTimer(ActionTimer, this, &UCombatComponent::FinishSwap, 1.5f, false);

	Character->PlayAnim(SwapMontage);
	
}

void UCombatComponent::FinishSwap()
{
	CombatState = ECombatState::ECS_Unoccupied;
	ActionEnd();
}

void UCombatComponent::FinishSwapAttachWeapons()
{
	if (Character == nullptr) return;
	AWeapon* TempWeapon = EquippedWeapon;
	EquippedWeapon = SecondaryWeapon;
	SecondaryWeapon = TempWeapon;

	EquippedWeapon->OnEquipped(Character);
	SecondaryWeapon->OnEquippedSecondary(Character);
	
	SetWeaponType();
	UpdateCarriedAmmo();
	ReloadEmptyWeapon();
	
	NotifyAmmo();
	NotifyWeapon();
}


void UCombatComponent::Fire()
{
	if (!CanFire()) return;
	
	Super::Fire();
	NotifyAmmo();
	
	CrosshairShootingFactor = .75f;
}

void UCombatComponent::LocalFireWeapon(const TArray<FVector_NetQuantize>& TraceHitTargets, const bool IsSimulate) const
{
	Super::LocalFireWeapon(TraceHitTargets, IsSimulate);

	if (EquippedRangeWeapon)
	{
		Character->PlayAnim(FireWeaponMontage, bAiming ? FName("FireAim") : FName("FireHip"));
		return;
	}
	
	Character->PlayAnim(MeleeWeaponMontage,
		EquippedWeapon->GetWeaponType() == EWeaponType::EWT_MeleeWeaponBlunt ? FName("Attack1") : FName("Attack2"));
	
}

void UCombatComponent::FinishFire()
{
	Super::FinishFire();
	ActionEnd();
}

// Reload

void UCombatComponent::Reload()
{
	if (CarriedAmmo <= 0) return;

	Super::Reload();
}

void UCombatComponent::FinishReload()
{
	Super::FinishReload();
	
	if (Character == nullptr || EquippedRangeWeapon == nullptr) return;

	EquippedWeapon->Reload(this);
	
	NotifyAmmo();
	ActionEnd();
	
}

// Set Aiming
void UCombatComponent::SetAiming(bool bIsAiming)
{
	if (Character == nullptr || EquippedWeapon == nullptr) return;
	if (bAiming == bIsAiming) return;
	
	Super::SetAiming(bIsAiming);
	
	if (!Character->IsLocallyControlled()) return;
	
	EMovementState NewState = bIsAiming ? EMovementState::EMS_Aim : EMovementState::EMS_Idle;
	Character->SetMovementState(NewState);

	if (!bIsAiming)
	{
		EquippedWeapon->SetStateUnAiming();
		return;
	}

	TObjectPtr<APlayerCharacter> Player = Cast<APlayerCharacter>(Character);

	if (Player == nullptr) return;

	EquippedWeapon->SetStateAiming(Player);

}

void UCombatComponent::ServerSetAiming(bool bIsAiming)
{
	Super::ServerSetAiming_Implementation(bIsAiming);
	if (Character)
	{
		EMovementState NewState = bIsAiming ? EMovementState::EMS_Aim : EMovementState::EMS_Idle;
		Character->SetMovementState(NewState);
	}
}

// Set Crosshair
void UCombatComponent::SetHUDCrosshair(float DeltaTime, const FLinearColor& CrosshairColor)
{
	if (Character == nullptr || Character->Controller == nullptr) return;
	if (!EquippedWeapon) return;

	if (!HUD)
	{
		const APlayerController* Controller = Cast<APlayerController>(Character->Controller);
		if (!Controller) return;

		HUD =  Cast<APrepperHUD>(Controller->GetHUD());
	}
	
	if (!HUD) return;
	
	FHUDPackage HUDPackage;
	
	EquippedWeapon->GetCrosshair(DeltaTime, bAiming, HUDPackage);

	HUDPackage.CrosshairColor = CrosshairColor;
	
	HUD->SetHUDPackage(HUDPackage);
}

// Targeting

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
	const FVector End = Start + CrosshairWorldDirection * TRACE_LEN;

	if (Character)
	{
		const float DistanceToCharacter = (Character->GetActorLocation() - Start).Size();
		Start += CrosshairWorldDirection * (DistanceToCharacter + 100.f);
	}
	

	const bool bHitSomething = GetWorld()->LineTraceSingleByChannel(
		TraceHitResult, Start, End, ECC_Visibility);
	
	if (!bHitSomething)
	{
		// TraceHitResult의 Location에 End 좌표 설정
		TraceHitResult.Location = End;
	}
}

void UCombatComponent::TargetElim()
{
	Super::TargetElim();
		
	if(SecondaryWeapon)
	{
		SecondaryWeapon->OnDropped(Character);
	}

	if (EquippedWeapon == nullptr) return;
	
	if (!Character->IsLocallyControlled()) return;
	
	EquippedWeapon->SetStateUnAiming();
}

void UCombatComponent::OnRep_Ammo()
{
	ConvertArrayToMap(); // 클라이언트에서 배열을 맵으로 변환
}

FGaugeInt UCombatComponent::GetAmmoShow()
{
	int WeaponAmmo = -1;
	
	if (EquippedWeapon)
	{
		WeaponAmmo = EquippedWeapon->GetLeftAmmo();
	}
		
	return FGaugeInt(WeaponAmmo, CarriedAmmo);
}

// Ammo
void UCombatComponent::AddAmmo(EWeaponType WeaponType, int32 AmmoAmount)
{
	if (CarriedAmmoMap.Contains(WeaponType))
	{
		CarriedAmmoMap[WeaponType] = FMath::Clamp(CarriedAmmoMap[WeaponType] + AmmoAmount, 0, MaxCarriedAmmo);
		UpdateCarriedAmmo();
	}

	ReloadEmptyWeapon();
}

int UCombatComponent::UseAmmo(EWeaponType WeaponType, int32 MaxUse)
{
	if (!CarriedAmmoMap.Contains(WeaponType))
	{
		return 0;
	}
	
	int Retval = CarriedAmmoMap[WeaponType];
	if (Retval > MaxUse) Retval = MaxUse;

	CarriedAmmoMap[WeaponType] -= Retval;
	NotifyAmmo();
	
	return Retval;
}

void UCombatComponent::OnRep_CarriedAmmo()
{
	NotifyAmmo();
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
	
	NotifyAmmo();
	ConvertMapToArray();
}

void UCombatComponent::ConvertMapToArray()
{
	ReplicatedWeaponAmmoData.Empty();
	for (const auto& Elem : CarriedAmmoMap)
	{
		FWeaponConvertData ItemData;
		ItemData.WeaponType = Elem.Key;
		ItemData.Count = Elem.Value;
		ReplicatedWeaponAmmoData.Add(ItemData);
	}
}

void UCombatComponent::ConvertArrayToMap()
{
	CarriedAmmoMap.Empty();
	for (const auto& Elem : ReplicatedWeaponAmmoData)
	{
		CarriedAmmoMap.Add(Elem.WeaponType, Elem.Count);
	}
}
