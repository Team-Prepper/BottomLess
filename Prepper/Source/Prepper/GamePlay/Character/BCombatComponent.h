// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Prepper/_Base/ObserverPattern/Subject.h"
#include "BCombatComponent.generated.h"


class ABCharacter;
class AWeapon;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class PREPPER_API UBCombatComponent : public UActorComponent, public ISubject<UBCombatComponent>
{
	GENERATED_BODY()

protected:
	UPROPERTY()
	TObjectPtr<ABCharacter> TargetCharacter;
	
public:
	// Sets default values for this component's properties
	UBCombatComponent();

	void SetTargetCharacter(TObjectPtr<ABCharacter> Character);
	
	virtual void EquipWeapon(AWeapon* Weapon) {}
	virtual void AttackTrigger(bool IsTrigger) {}
	virtual void AimTrigger(bool IsTrigger) {}
	virtual void Reload() {}

	virtual void Swap() PURE_VIRTUAL();
	
	virtual FString GetEquippedWeaponCode() const { return FString(); }
	virtual FString GetAmmoValue() const { return FString(); }

private:
	TSet<IObserver<UBCombatComponent>*> Observers;
public:
	virtual void Attach(IObserver<UBCombatComponent>* Observer) override;
	virtual void Detach(IObserver<UBCombatComponent>* Observer) override;
	virtual void Notify();
	
};
