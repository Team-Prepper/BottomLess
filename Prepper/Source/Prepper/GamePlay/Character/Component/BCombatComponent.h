// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Prepper/__Base/ObserverPattern/Subject.h"
#include "BCombatComponent.generated.h"


class UPlayerOverlay;
class ABCharacter;
class AWeapon;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class PREPPER_API UBCombatComponent : public UActorComponent, public ISubject<UBCombatComponent>
{
	GENERATED_BODY()
private:
	
	UPROPERTY(EditAnywhere, Category = Combat)
	TObjectPtr<UAnimMontage> ReloadMontage;
	
	FTimerHandle ActionTimer;
	
protected:
	UPROPERTY()
	TObjectPtr<ABCharacter> TargetCharacter;
	UPROPERTY()
	TObjectPtr<UPlayerOverlay> TargetOverlay;

	TObjectPtr<AWeapon> EquippedWeapon;
	TObjectPtr<AWeapon> SecondaryWeapon;
	
	bool IsAttack;
	bool IsAttackNow;
	bool IsAimingLocal;
	bool IsReload;

	FVector TraceHit() const;
	virtual void SetEquippedAmmo(int AmmoCnt);

	void EquippedAct(TObjectPtr<AWeapon> Weapon);
	void SecondaryEquippedAct(TObjectPtr<AWeapon> Weapon);
	
	void TryAttack();
	virtual void FireWeaponToTargets(const TArray<FVector_NetQuantize>& TraceHitTargets) const;
	void FinishAttack();
	
	void TryReload();
	virtual void ReloadAct();
	void FinishReload();
	
public:
	// Sets default values for this component's properties
	UBCombatComponent();
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void SetCrosshairOverlay(TObjectPtr<UPlayerOverlay> Overlay);
	
	void SetTargetCharacter(TObjectPtr<ABCharacter> Character);
	virtual void EquipWeapon(TObjectPtr<ABCharacter> Target, TObjectPtr<AWeapon> Weapon);
	virtual void AttackTrigger(bool IsTrigger);
	virtual void AimTrigger(bool IsTrigger) {}
	void AimingAct(bool IsTrigger) const;
	
	virtual void Reload();

	virtual void Swap();
	virtual void CharacterElim();
	
	virtual TObjectPtr<AWeapon> GetEquippedWeapon() const { return EquippedWeapon; }
	virtual TObjectPtr<AWeapon> GetSecondaryWeapon() const { return SecondaryWeapon; }
	
	virtual FString GetAmmoValue() const { return FString(); }

private:
	TSet<IObserver<UBCombatComponent>*> Observers;
public:
	virtual void Attach(IObserver<UBCombatComponent>* Observer) override;
	virtual void Detach(IObserver<UBCombatComponent>* Observer) override;
	virtual void Notify();
	
};
