// Fill out your copyright notice in the Description page of Project Settings.

#include "UnrealCombatComponent.h"

#include "Net/UnrealNetwork.h"
#include "Prepper/GamePlay/Character/BCharacter.h"
#include "Prepper/GamePlay/Weapon/Weapon.h"

// Sets default values for this component's properties
UUnrealCombatComponent::UUnrealCombatComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	SetIsReplicated(true);

	TargetCharacter = nullptr;
	EquippedWeapon = nullptr;
	EquippedAmmo = 0;

	IsAiming = false;
	IsAttack = false;
	IsAttackNow = false;
	IsReload = false;

	IsAimingLocal = false;
}

void UUnrealCombatComponent::Swap()
{
	if (SecondaryWeapon == nullptr) return;
	const TObjectPtr<AWeapon> Temp = EquippedWeapon;
	EquippedWeapon = SecondaryWeapon;
	SecondaryWeapon = Temp;

	EquippedWeapon->OnEquipped(TargetCharacter);
	SecondaryWeapon->OnEquippedSecondary(TargetCharacter);
	
	Notify();
}

// Called when the game starts
void UUnrealCombatComponent::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void UUnrealCombatComponent::TickComponent(float DeltaTime, ELevelTick TickType,
                                           FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void UUnrealCombatComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(UUnrealCombatComponent, IsAiming);
	DOREPLIFETIME(UUnrealCombatComponent, EquippedWeapon);
	DOREPLIFETIME(UUnrealCombatComponent, EquippedAmmo);
	DOREPLIFETIME(UUnrealCombatComponent, SecondaryWeapon);
	DOREPLIFETIME(UUnrealCombatComponent, DroppedWeapon);
}

void UUnrealCombatComponent::EquipWeapon(AWeapon* Weapon)
{
	if (EquippedWeapon != nullptr && SecondaryWeapon == nullptr)
	{
		SecondaryWeapon = Weapon;
		SecondaryWeapon->OnEquippedSecondary(TargetCharacter);
		return;
	}
	
	DroppedWeapon = EquippedWeapon;
	
	if (DroppedWeapon != nullptr)
	{
		DroppedWeapon->OnDropped(TargetCharacter);
	}
	
	EquippedWeapon = Weapon;
	EquippedWeapon->OnEquipped(TargetCharacter);
	EquippedAmmo = EquippedWeapon->GetLeftAmmo();

	Notify();
	
	TryAttack();
	
}

void UUnrealCombatComponent::OnRep_Aiming()
{
	if (GetOwner()->GetInstigatorController() &&
		GetOwner()->GetInstigatorController()->IsLocalController()) return;
	AimingAct(IsAiming);
}

void UUnrealCombatComponent::OnRep_Ammo()
{
	EquippedWeapon->SetLeftAmmo(EquippedAmmo);

	Notify();
}

void UUnrealCombatComponent::OnRep_EquippedWeapon()
{
	if (!EquippedWeapon) return;

	EquippedWeapon->OnEquipped(TargetCharacter);
	Notify();
}

void UUnrealCombatComponent::OnRep_SecondaryWeapon()
{
	if (!SecondaryWeapon) return;
	SecondaryWeapon->OnEquippedSecondary(TargetCharacter);
}

void UUnrealCombatComponent::OnRep_DroppedWeapon()
{
	if (!DroppedWeapon) return;
	DroppedWeapon->OnDropped(TargetCharacter);
}

void UUnrealCombatComponent::MulticastAttackWeapon_Implementation(
	const TArray<FVector_NetQuantize>& TraceHitTargets) const
{
	EquippedWeapon->Fire(TraceHitTargets, GetOwner()->GetInstigatorController(), !GetOwner()->HasAuthority());
}

void UUnrealCombatComponent::AimingAct(const bool IsTrigger) const
{
	const TObjectPtr<ABCharacter> Target = TargetCharacter;

	if (Target == nullptr) return;
	Target->AimTrigger(IsTrigger);
}

void UUnrealCombatComponent::FinishAttack()
{
	EquippedAmmo = EquippedWeapon->GetLeftAmmo();
	Notify();
	IsAttackNow = false;
	TryAttack();
}

void UUnrealCombatComponent::TryAttack()
{
	if (EquippedWeapon == nullptr) return;
	if (!IsAttack) return;
	if (IsAttackNow) return;
	if (IsReload) return;
	if (!EquippedWeapon->CanAttack()) return;

	AttackAct();
}

void UUnrealCombatComponent::TryReload()
{
	if (EquippedWeapon == nullptr) return;
	if (!EquippedWeapon->CanReload()) return;
	if (IsAttackNow) return;
	if (IsReload) return;

	ReloadAct();
}

void UUnrealCombatComponent::AttackAct()
{
	IsAttackNow = true;

	FVector HitTarget = TraceHit();

	const TArray<FVector_NetQuantize> HitTargets = EquippedWeapon->GetTarget(HitTarget);

	MulticastAttackWeapon(HitTargets);

	GetOwner()->GetWorldTimerManager().SetTimer(
		ActionTimer,
		this,
		&UUnrealCombatComponent::FinishAttack,
		EquippedWeapon->GetFireDelay()
	);
}

void UUnrealCombatComponent::ReloadAct()
{
	IsReload = true;
	GetOwner()->GetWorldTimerManager().SetTimer(
		ActionTimer,
		this,
		&UUnrealCombatComponent::FinishReload,
		EquippedWeapon->GetFireDelay()
	);
	EquippedWeapon->PlayReload(TargetCharacter, ReloadMontage);
}

void UUnrealCombatComponent::FinishReload()
{
	EquippedWeapon->Reload(TargetCharacter->GetAmmoBox());
	EquippedAmmo = EquippedWeapon->GetLeftAmmo();
	IsReload = false;

	Notify();
}

FVector UUnrealCombatComponent::TraceHit() const
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

void UUnrealCombatComponent::AimTrigger(bool IsTrigger)
{
	IsAimingLocal = IsTrigger;
	ServerAimTrigger(IsAimingLocal);
	AimingAct(IsAimingLocal);
}

void UUnrealCombatComponent::AttackTrigger(bool IsTrigger)
{
	IsAttack = IsTrigger;
	ServerAttackTrigger(IsAttack);
}

void UUnrealCombatComponent::Reload()
{
	ServerReload();
}

FString UUnrealCombatComponent::GetEquippedWeaponCode() const
{
	if (EquippedWeapon == nullptr) return FString();
	return EquippedWeapon->GetCode();
}

FString UUnrealCombatComponent::GetAmmoValue() const
{
	if (EquippedWeapon == nullptr) return FString("- / -");
	return EquippedWeapon->GetAmmoValue();
}

void UUnrealCombatComponent::ServerAimTrigger_Implementation(bool IsTrigger)
{
	IsAiming = IsTrigger;
}

void UUnrealCombatComponent::ServerAttackTrigger_Implementation(bool IsTrigger)
{
	IsAttack = IsTrigger;
	TryAttack();
}

void UUnrealCombatComponent::ServerReload_Implementation()
{
	TryReload();
}