// Fill out your copyright notice in the Description page of Project Settings.


#include "BCombatComponent.h"

#include "GameFramework/CharacterMovementComponent.h"
#include "Prepper/GamePlay/Character/BCharacter.h"
#include "Prepper/GamePlay/Character/CharacterAnimInstance.h"
#include "Prepper/GamePlay/UI/PlayerOverlay.h"
#include "Prepper/GamePlay/Weapon/Weapon.h"
#include "Prepper/___Legacy/HUD/PrepperHUD.h"

// Sets default values for this component's properties
UBCombatComponent::UBCombatComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	
	TargetCharacter = nullptr;
	
	IsAimingLocal = false;
	IsAttack = false;
	IsAttackNow = false;
	IsReload = false;
}

void UBCombatComponent::SetEquippedAmmo(const int AmmoCnt)
{
	if (EquippedWeapon == nullptr) return;
	
	EquippedWeapon->SetLeftAmmo(AmmoCnt);
	Notify();
}

void UBCombatComponent::EquippedAct(const TObjectPtr<AWeapon> Weapon)
{
	EquippedWeapon = Weapon;
	
	TargetCharacter->bUseControllerRotationYaw = EquippedWeapon != nullptr;
	TargetCharacter->GetCharacterMovement()->bOrientRotationToMovement = EquippedWeapon == nullptr;
	Cast<UCharacterAnimInstance>(TargetCharacter->GetMesh()->GetAnimInstance())->SetEquippedWeapon(EquippedWeapon);
	
	if (EquippedWeapon == nullptr) return;
	
	EquippedWeapon->OnEquipped(TargetCharacter);
	Notify();
}

void UBCombatComponent::SecondaryEquippedAct(const TObjectPtr<AWeapon> Weapon)
{
	SecondaryWeapon = Weapon;
	
	if (SecondaryWeapon == nullptr) return;
	
	SecondaryWeapon->OnEquippedSecondary(TargetCharacter);
}

FVector UBCombatComponent::TraceHit() const
{
	FVector Start;
	FVector Direction;

	TargetCharacter->GetLookDirection(Start, Direction);
	float TraceDistance = 80000.f; // 트레이스 거리 (1,000 유닛)
	FVector End = Start + (Direction * TraceDistance);

	FCollisionQueryParams TraceParams;
	TraceParams.bTraceComplex = true; // 복잡한 충돌 확인 여부
	TraceParams.AddIgnoredActor(TargetCharacter); // 자신은 충돌 무시

	if (FHitResult HitResult; GetWorld()->LineTraceSingleByChannel(
		HitResult, // 충돌 결과 저장
		Start, // 시작 위치
		End, // 끝 위치
		ECC_Visibility, // 충돌 채널
		TraceParams // 쿼리 매개변수
	))
	{
		return HitResult.Location;
	}
	return End;
}

void UBCombatComponent::TryAttack()
{
	if (EquippedWeapon == nullptr) return;
	if (!IsAttack) return;
	if (IsAttackNow) return;
	if (IsReload) return;
	if (!EquippedWeapon->CanAttack()) return;
	
	IsAttackNow = true;

	GetOwner()->GetWorldTimerManager().SetTimer(
		ActionTimer,
		this,
		&UBCombatComponent::FinishAttack,
		EquippedWeapon->GetFireDelay()
	);

	FVector HitTarget = TraceHit();

	const TArray<FVector_NetQuantize> HitTargets = EquippedWeapon->GetTarget(HitTarget);

	FireWeaponToTargets(HitTargets);
}

void UBCombatComponent::FireWeaponToTargets(const TArray<FVector_NetQuantize>& TraceHitTargets) const
{
	EquippedWeapon->Fire(TraceHitTargets, GetOwner()->GetInstigatorController(), !GetOwner()->HasAuthority());
}

void UBCombatComponent::FinishAttack()
{
	SetEquippedAmmo(EquippedWeapon->GetLeftAmmo());
	IsAttackNow = false;
	TryAttack();
}

void UBCombatComponent::TryReload()
{
	if (EquippedWeapon == nullptr) return;
	if (!EquippedWeapon->CanReload()) return;
	if (IsAttackNow) return;
	if (IsReload) return;
	
	IsReload = true;
	GetOwner()->GetWorldTimerManager().SetTimer(
		ActionTimer,
		this,
		&UBCombatComponent::FinishReload,
		EquippedWeapon->GetFireDelay()
	);
	
	ReloadAct();
}

void UBCombatComponent::ReloadAct()
{
	EquippedWeapon->PlayReload(TargetCharacter, ReloadMontage);
}

void UBCombatComponent::FinishReload()
{
	EquippedWeapon->Reload(TargetCharacter->GetAmmoBox());
	SetEquippedAmmo(EquippedWeapon->GetLeftAmmo());
	IsReload = false;

	Notify();
}

void UBCombatComponent::TickComponent(float DeltaTime, ELevelTick TickType,
	FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	
	if (TargetOverlay == nullptr) return;
	if (EquippedWeapon == nullptr) return;
	
	FHUDPackage HUDPackage;
	EquippedWeapon->GetCrosshair(DeltaTime, IsAimingLocal, HUDPackage);
	TargetOverlay->DrawCrosshair(HUDPackage);
	
}

void UBCombatComponent::SetCrosshairOverlay(TObjectPtr<UPlayerOverlay> Overlay)
{
	TargetOverlay = Overlay;
}

void UBCombatComponent::SetTargetCharacter(TObjectPtr<ABCharacter> Character)
{
	TargetCharacter = Character;
}

void UBCombatComponent::EquipWeapon(TObjectPtr<ABCharacter> Target, TObjectPtr<AWeapon> Weapon)
{
	TargetCharacter = Target;
	
	if (EquippedWeapon != nullptr && SecondaryWeapon == nullptr)
	{
		SecondaryEquippedAct(Weapon);
		return;
	}

	if (EquippedWeapon != nullptr)
	{
		EquippedWeapon->OnDropped();
	}

	EquippedAct(Weapon);
	SetEquippedAmmo(EquippedWeapon->GetLeftAmmo());

	Notify();
	TryAttack();
}

void UBCombatComponent::AttackTrigger(const bool IsTrigger)
{
	IsAttack = IsTrigger;
	TryAttack();
}

void UBCombatComponent::Reload()
{
	TryReload();
}

void UBCombatComponent::Swap()
{
	if (SecondaryWeapon == nullptr) return;
	
	const TObjectPtr<AWeapon> Temp = EquippedWeapon;
	
	EquippedWeapon = SecondaryWeapon;
	SecondaryWeapon = Temp;
	
	EquippedWeapon->OnEquipped(TargetCharacter);
	SecondaryWeapon->OnEquippedSecondary(TargetCharacter);
	
	Notify();
}

void UBCombatComponent::AimingAct(const bool IsTrigger) const
{
	if (TargetCharacter == nullptr) return;
	TargetCharacter->AimTrigger(IsTrigger);
}

void UBCombatComponent::CharacterElim()
{
	if (EquippedWeapon != nullptr)
	{
		EquippedWeapon->OnDropped();
	}
	if (SecondaryWeapon != nullptr)
	{
		SecondaryWeapon->OnDropped();
	}
}

void UBCombatComponent::Attach(IObserver<UBCombatComponent>* Observer)
{
	Observers.Add(Observer);;
	Observer->Update(*this);
}

void UBCombatComponent::Detach(IObserver<UBCombatComponent>* Observer)
{
	Observers.Remove(Observer);
}

void UBCombatComponent::Notify()
{
	for (const auto Observer : Observers)
	{
		Observer->Update(*this);
	}
}