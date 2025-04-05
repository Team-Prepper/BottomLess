#pragma once

#include <set>

#include "CoreMinimal.h"
#include "../CharacterComponent.h"
#include "Components/ActorComponent.h"
#include "Prepper/__Legacy/Character/Enums/CombatState.h"
#include "Prepper/_Base/ObserverPattern/Subject.h"
#include "Prepper/_Base/Util/GaugeInt.h"
#include "Prepper/_Base/Util/GaugeValue.h"
#include "Prepper/__Legacy/Weapon/WeaponHandler.h"
#include "Prepper/__Legacy/Weapon/WeaponTypes.h"
#include "BaseCombatComponent.generated.h"

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PREPPER_API UBaseCombatComponent : public UCharacterComponent,
									 public IWeaponHandler, public ISubject<GaugeValue<int>>, public ISubject<FString>
{
	GENERATED_BODY()
	friend class ABaseCharacter;
	friend class APlayerCharacter;
	friend class ASurvivorGameMode;
// Actor
public:	
	UBaseCombatComponent();
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

// Observer Pattern
private:
	std::pmr::set<IObserver<GaugeValue<int>>*> AmmoObservers;
	std::pmr::set<IObserver<FString>*> WeaponObservers;
	virtual FGaugeInt GetAmmoShow() { return FGaugeInt(0, 0); }
	
public:
	virtual void Attach(IObserver<GaugeValue<int>>* Observer) override;
	virtual void Detach(IObserver<GaugeValue<int>>* Observer) override;
	virtual void Attach(IObserver<FString>* Observer) override;
	virtual void Detach(IObserver<FString>* Observer) override;
	virtual void NotifyAmmo();
	virtual void NotifyWeapon();
	
// Equip Weapon
protected:
	UPROPERTY(ReplicatedUsing = OnRep_EquippedWeapon)
	AWeaponActor* EquippedWeapon;
	
public:
	virtual void EquipWeapon(AWeaponActor* WeaponToEquip) override;
	void EquipWeaponSet(AWeaponActor* WeaponToEquip);

protected:
	virtual void DropEquippedWeapon();
	UFUNCTION()
	virtual void OnRep_EquippedWeapon();
	
protected:
	virtual void ActionEnd();
// Fire
protected:
	//Auto Fire
	FTimerHandle ActionTimer;
	bool bFireButtonPressed;
	
	UPROPERTY(EditAnywhere, Category = "Combat")
	UAnimMontage* AttackMontage;

public:
	virtual void FireTrigger(bool IsTrigger) override;
	
	virtual void Fire() override;

protected:
	
	bool CanFire() const;
	
	virtual void LocalFireWeapon(const TArray<FVector_NetQuantize>& TraceHitTargets, const bool IsSimulate) const;

	UFUNCTION(Server, Reliable)
	void ServerFireWeapon(const TArray<FVector_NetQuantize>& TraceHitTargets) const;

	UFUNCTION(NetMulticast, Reliable)
	void MulticastFireWeapon(const TArray<FVector_NetQuantize>& TraceHitTargets) const;
	
	virtual void FinishFire();

// Reload
protected:
	UPROPERTY(EditAnywhere, Category = Combat)
	UAnimMontage* ReloadMontage;
	
	bool bLocallyReload = false;
	
public:
	virtual void Reload() override;
	virtual void PickupAmmo(EWeaponType WeaponType, int32 AmmoAmount)
	{
		
	}
	void HandleReload() const;
	
protected:
	void ReloadEmptyWeapon();

	UFUNCTION(Server, Reliable)
	void ServerReload();
	
	UFUNCTION(BlueprintCallable)
	virtual void FinishReload();

// Set Aiming
protected:
	bool bAimButtonPressed = false;
	
	UPROPERTY(ReplicatedUsing = OnRep_Aiming)
	bool bAiming;
	
public:
	virtual void SetAiming(bool bIsAiming) override;

private:
	UFUNCTION()
	void OnRep_Aiming();
	
	UFUNCTION(Server, Reliable)
	virtual void ServerSetAiming(bool bIsAiming);

// Targeting
protected:
	
	UPROPERTY()
	ABaseCharacter* Character;
	
	TArray<FVector_NetQuantize> HitTargets;

public:
	virtual void SetCharacter(ABaseCharacter* Target) override;
	virtual void TargetElim() override;
	
	FVector HitTarget;
// Combat State
protected:
	UPROPERTY(ReplicatedUsing = OnRep_CombatState)
	ECombatState CombatState = ECombatState::ECS_Unoccupied;
	UFUNCTION()
	void OnRep_CombatState();
public:
	ECombatState GetCombatState() const { return CombatState; }

private:
	FHitResult TraceHit();
};