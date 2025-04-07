// Fill out your copyright notice in the Description page of Project Settings.

#include "UnrealCombatComponent.h"

#include "Net/UnrealNetwork.h"
#include "Prepper/GamePlay/Character/BCharacter.h"
#include "Prepper/GamePlay/CharacterController/CharacterController.h"
#include "Prepper/__Legacy/Weapon/WeaponActor.h"

class ICharacterController;
// Sets default values for this component's properties
UUnrealCombatComponent::UUnrealCombatComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

	TargetCC = nullptr;
	EquippedWeapon = nullptr;

	IsAiming = false;
	IsAttack = false;
	IsAttackNow = false;

	IsAimingLocal = false;
}

void UUnrealCombatComponent::SetTargetCC(ICharacterController* CC)
{
	TargetCC = CC;
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
}

void UUnrealCombatComponent::EquipWeapon(AWeaponActor* Weapon)
{
	EquippedWeapon = Weapon;
	EquippedWeapon->OnEquipped(TargetCC->GetTargetCharacter());
	TryAttack();
}

void UUnrealCombatComponent::OnRep_Aiming()
{
	if (GetOwner<APlayerController>()->IsLocalController()) return;
	AimingAct(IsAiming);
}

void UUnrealCombatComponent::OnRep_EquippedWeapon()
{
	if (!EquippedWeapon) return;

	EquippedWeapon->OnEquipped(TargetCC->GetTargetCharacter());
}

void UUnrealCombatComponent::MulticastFireWeapon_Implementation(
	const TArray<FVector_NetQuantize>& TraceHitTargets) const
{
	EquippedWeapon->Fire(TraceHitTargets, GetOwner<AController>(), !GetOwner()->HasAuthority());
}

void UUnrealCombatComponent::AimingAct(bool IsTrigger)
{
	const TObjectPtr<ABCharacter> Target = TargetCC->GetTargetCharacter();

	if (Target == nullptr) return;
	Target->AimTrigger(IsTrigger);
}

void UUnrealCombatComponent::FinishFire()
{
	TryAttack();
	IsAttackNow = false;
}

void UUnrealCombatComponent::TryAttack()
{
	if (!IsAttack) return;
	if (IsAttackNow) return;
	if (!EquippedWeapon->CanAttack()) return;

	AttackAct();
}

void UUnrealCombatComponent::AttackAct()
{
	IsAttackNow = true;

	FVector HitTarget = TraceHit();

	const TArray<FVector_NetQuantize> HitTargets = EquippedWeapon->GetTarget(HitTarget);

	MulticastFireWeapon(HitTargets);

	GetOwner()->GetWorldTimerManager().SetTimer(
		ActionTimer,
		this,
		&UUnrealCombatComponent::FinishFire,
		EquippedWeapon->GetFireDelay()
	);
}

FVector UUnrealCombatComponent::TraceHit() const
{
	FVector Start;
	FVector Direction;

	TargetCC->GetTargetCharacter()->GetLookDirection(Start, Direction);
	float TraceDistance = 80000.f; // 트레이스 거리 (1,000 유닛)
	FVector End = Start + (Direction * TraceDistance);

	FCollisionQueryParams TraceParams;
	TraceParams.bTraceComplex = true; // 복잡한 충돌 확인 여부
	TraceParams.AddIgnoredActor(TargetCC->GetTargetCharacter()); // 자신은 충돌 무시

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

void UUnrealCombatComponent::ServerAimTrigger_Implementation(bool IsTrigger)
{
	IsAiming = IsTrigger;
}

void UUnrealCombatComponent::ServerAttackTrigger_Implementation(bool IsTrigger)
{
	IsAttack = IsTrigger;

	if (EquippedWeapon == nullptr) return;
	TryAttack();
}

void UUnrealCombatComponent::ServerReload_Implementation()
{
}
