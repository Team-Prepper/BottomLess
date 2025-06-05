#include "BaseCombatComponent.h"

#include "GameFramework/CharacterMovementComponent.h"
#include "Net/UnrealNetwork.h"
#include "Prepper/___Legacy/Character/PlayerCharacter.h"
#include "Prepper/GamePlay/Weapon/Weapon.h"

// Actor
UBaseCombatComponent::UBaseCombatComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

	EquippedWeapon = nullptr;
	
	bFireButtonPressed = false;
	bAiming = false;

	ReloadMontage = nullptr;
	AttackMontage = nullptr;

	Character = nullptr;
}

void UBaseCombatComponent::TickComponent(float DeltaTime, ELevelTick TickType,
                                         FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	HitTarget = TraceHit().Location;
	
}

void UBaseCombatComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UBaseCombatComponent, EquippedWeapon);
	DOREPLIFETIME(UBaseCombatComponent, bAiming);
	DOREPLIFETIME(UBaseCombatComponent, CombatState);
}


// Observer Pattern

void UBaseCombatComponent::Attach(IObserver<GaugeValue<int>>* Observer)
{
	AmmoObservers.insert(Observer);

	if (!EquippedWeapon)
	{
		Observer->Update(FGaugeInt(-1, -1));
		return;
	}
	Observer->Update(GetAmmoShow());
}

void UBaseCombatComponent::Detach(IObserver<GaugeValue<int>>* Observer)
{
	AmmoObservers.erase(Observer);
}

void UBaseCombatComponent::NotifyAmmo()
{
	const FGaugeInt Value = GetAmmoShow();
	std::ranges::for_each(AmmoObservers, [&](IObserver<GaugeValue<int>>* Observer) {
		Observer->Update(Value);
	});
}

void UBaseCombatComponent::Attach(IObserver<FString>* Observer)
{
	WeaponObservers.insert(Observer);

	if (!EquippedWeapon)
	{
		Observer->Update(FString(""));
		return;
	}
	Observer->Update(EquippedWeapon->GetCode());
}

void UBaseCombatComponent::Detach(IObserver<FString>* Observer)
{
	WeaponObservers.erase(Observer);
}

void UBaseCombatComponent::NotifyWeapon()
{
	FString Value = FString();
	
	if (EquippedWeapon)
	{
		Value = EquippedWeapon->GetCode();
	}
	
	std::ranges::for_each(WeaponObservers, [&](IObserver<FString>* Observer) {
		Observer->Update(Value);
	});
	
}
// Equipped Weapon
void UBaseCombatComponent::EquipWeapon(AWeapon* WeaponToEquip)
{
	if (Character == nullptr || WeaponToEquip == nullptr) return;
	if (CombatState != ECombatState::ECS_Unoccupied) return;

	if (WeaponToEquip == nullptr) return;
	
	DropEquippedWeapon();
	
	EquippedWeapon = WeaponToEquip;
	EquippedWeapon->SetOwner(Character);
	EquippedWeapon->OnEquipped(Character);
	EquippedWeapon->SetWeaponHandler(this);
	
	ReloadEmptyWeapon();
	NotifyAmmo();
	NotifyWeapon();
}

void UBaseCombatComponent::EquipWeaponSet(AWeapon* WeaponToEquip)
{
	EquippedWeapon = WeaponToEquip;
	EquippedWeapon->SetOwner(Character);
}

void UBaseCombatComponent::DropEquippedWeapon()
{
	if (!EquippedWeapon) return;

	EquippedWeapon->OnDropped(Character);
	EquippedWeapon = nullptr;
}

void UBaseCombatComponent::OnRep_EquippedWeapon()
{
	NotifyWeapon();
	
	if (!EquippedWeapon) return;
	if (!Character) return;

	EquippedWeapon->OnEquipped(Character);

	Character->GetCharacterMovement()->bOrientRotationToMovement = false;
	Character->bUseControllerRotationYaw = true;
}

void UBaseCombatComponent::ActionEnd()
{
	ReloadEmptyWeapon();
	Fire();
}

// Fire
void UBaseCombatComponent::FireTrigger(bool bPressed)
{
	if (bFireButtonPressed == bPressed) return;
	
	bFireButtonPressed = bPressed;

	if (bFireButtonPressed)
	{
		Fire();
	}
}

void UBaseCombatComponent::Fire()
{
	if (!CanFire()) return;
	
	HitTargets = EquippedWeapon->GetTarget(HitTarget);
	
	LocalFireWeapon(HitTargets, !Character->IsLocallyControlled());

	TArray<FVector_NetQuantize> NetworkHitTargets;

	EquippedWeapon->GetTarget(HitTarget);
	ServerFireWeapon(HitTargets);

	CombatState = ECombatState::ECS_Fire;

	Character->GetWorldTimerManager().SetTimer(
		ActionTimer,
		this,
		&UBaseCombatComponent::FinishFire,
		EquippedWeapon->GetFireDelay()
	);
}

bool UBaseCombatComponent::CanFire() const
{
	if (!EquippedWeapon) return false;
	if (!bFireButtonPressed) return false;
	if (CombatState != ECombatState::ECS_Unoccupied) return false;

	return EquippedWeapon->CanAttack();
}

void UBaseCombatComponent::LocalFireWeapon(const TArray<FVector_NetQuantize>& TraceHitTargets, const bool IsSimulate) const
{
	if (EquippedWeapon == nullptr || Character == nullptr) return;
	
	if (CombatState != ECombatState::ECS_Unoccupied) return;
	
	EquippedWeapon->Fire(TraceHitTargets, GetOwner()->GetInstigatorController(), IsSimulate);
	Character->PlayAnim(AttackMontage);
	
}

void UBaseCombatComponent::ServerFireWeapon_Implementation(const TArray<FVector_NetQuantize>& TraceHitTargets) const
{
	// 노이즈 발생
	EquippedWeapon->MakeNoise(1, Character, FVector::ZeroVector);
	MulticastFireWeapon(TraceHitTargets);
}

void UBaseCombatComponent::MulticastFireWeapon_Implementation(const TArray<FVector_NetQuantize>& TraceHitTargets) const
{
	if (Character && Character->IsLocallyControlled()) return;
	LocalFireWeapon(TraceHitTargets, !Character->HasAuthority());
}

void UBaseCombatComponent::FinishFire()
{
	CombatState = ECombatState::ECS_Unoccupied;
	ActionEnd();
}

// Reload

void UBaseCombatComponent::Reload()
{
	if (CombatState == ECombatState::ECS_Reloading) return;
	if (!EquippedWeapon->CanReload()) return;
	
	ServerReload();
	HandleReload();
}

void UBaseCombatComponent::HandleReload() const
{
	if (!Character) return;

	EquippedWeapon->PlayReload(Character, ReloadMontage);
}

void UBaseCombatComponent::ReloadEmptyWeapon()
{
	if (EquippedWeapon == nullptr) return;
	
	Reload();
}

void UBaseCombatComponent::ServerReload_Implementation()
{
	if (Character == nullptr || EquippedWeapon == nullptr) return;
	if (!Character->IsLocallyControlled())
	{
		CombatState = ECombatState::ECS_Reloading;
		HandleReload();
	}
}

void UBaseCombatComponent::FinishReload()
{
	CombatState = ECombatState::ECS_Unoccupied;
	ActionEnd();
}

// Set Aiming
void UBaseCombatComponent::SetAiming(bool bIsAiming)
{
	if (Character == nullptr || EquippedWeapon == nullptr) return;
	if (bAiming == bIsAiming) return;

	bAiming = bIsAiming;
	ServerSetAiming(bIsAiming);
	
	if (!Character->IsLocallyControlled()) return;
	
	bAimButtonPressed = bIsAiming;
}

void UBaseCombatComponent::OnRep_Aiming()
{
	if (Character && Character->IsLocallyControlled())
	{
		bAiming = bAimButtonPressed;
	}
}

void UBaseCombatComponent::ServerSetAiming_Implementation(bool bIsAiming)
{
	bAiming = bIsAiming;
}

void UBaseCombatComponent::SetCharacter(ABaseCharacter* Target)
{
	Character = Target;
}

void UBaseCombatComponent::TargetElim()
{
	FireTrigger(false);
	
	if(EquippedWeapon)
	{
		EquippedWeapon->OnDropped(Character);
	}
}

// Combat State
void UBaseCombatComponent::OnRep_CombatState()
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
	default:
		break;
	}
}

FHitResult UBaseCombatComponent::TraceHit()
{
	FVector Start = GetOwner()->GetActorLocation(); 

	// 끝 위치 (총구 앞 직선 방향으로 거리 설정)
	FVector ForwardVector = GetOwner()->GetActorForwardVector(); // 액터의 앞쪽 방향 벡터
	float TraceDistance = 1000.0f; // 트레이스 거리 (1,000 유닛)
	FVector End = Start + (ForwardVector * TraceDistance);

	// 충돌 결과를 저장할 HitResult
	FHitResult HitResult;

	// 충돌을 위한 쿼리 매개변수 설정
	FCollisionQueryParams TraceParams;
	TraceParams.bTraceComplex = true; // 복잡한 충돌 확인 여부
	TraceParams.AddIgnoredActor(GetOwner()); // 자신은 충돌 무시

	// 라인 트레이스 실행
	bool bHit = GetWorld()->LineTraceSingleByChannel(
		HitResult, // 충돌 결과 저장
		Start,     // 시작 위치
		End,       // 끝 위치
		ECC_Visibility, // 충돌 채널
		TraceParams // 쿼리 매개변수
	);

	// 충돌 여부 판별
	if (bHit)
	{
		// 충돌한 오브젝트의 이름 출력
		//UE_LOG(LogTemp, Warning, TEXT("Hit Actor: %s"), *HitResult.GetActor()->GetName());

		// 충돌 지점을 시각화 (디버그 용도)
		//DrawDebugPoint(GetWorld(), HitResult.ImpactPoint, 10.0f, FColor::Red, false, 2.0f);
	}
	else
	{
		// 충돌하지 않았을 경우
		//UE_LOG(LogTemp, Warning, TEXT("No Hit Detected"));
	}

	// 트레이스 라인을 디버그로 시각화 (옵션)
	//DrawDebugLine(GetWorld(), Start, End, FColor::Green, false, 2.0f, 0, 1.0f);

	return HitResult;
}
