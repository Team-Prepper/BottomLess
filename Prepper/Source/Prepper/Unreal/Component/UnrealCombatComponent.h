// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Prepper/GamePlay/CharacterController/Combat.h"
#include "UnrealCombatComponent.generated.h"

class ICharacterController;
class AWeaponActor;
class UAnimMontage;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PREPPER_API UUnrealCombatComponent :
		public UActorComponent, public ICombat
{
	GENERATED_BODY()

	ICharacterController* TargetCC;
	
	UPROPERTY(ReplicatedUsing = OnRep_EquippedWeapon)
	TObjectPtr<AWeaponActor> EquippedWeapon;
	UPROPERTY(ReplicatedUsing = OnRep_SecondaryWeapon)
	TObjectPtr<AWeaponActor> SecondaryWeapon;
	UPROPERTY(ReplicatedUsing = OnRep_DroppedWeapon)
	TObjectPtr<AWeaponActor> DroppedWeapon;
	
	UPROPERTY(EditAnywhere, Category = Combat)
	TObjectPtr<UAnimMontage> ReloadMontage;
	
	UPROPERTY(ReplicatedUsing=OnRep_Aiming)
	bool IsAiming;
	UPROPERTY(ReplicatedUsing=OnRep_Ammo)
	int EquippedAmmo;
	
	bool IsAttack;
	bool IsReload;
	bool IsAimingLocal;
	
	FTimerHandle ActionTimer;
	bool IsAttackNow;

	UFUNCTION()
	void OnRep_Aiming();
	UFUNCTION()
	void OnRep_Ammo() const;
	UFUNCTION()
	virtual void OnRep_EquippedWeapon();
	UFUNCTION()
	virtual void OnRep_SecondaryWeapon();
	UFUNCTION()
	virtual void OnRep_DroppedWeapon();
	
	UFUNCTION(NetMulticast, Reliable)
	void MulticastAttackWeapon(const TArray<FVector_NetQuantize>& TraceHitTargets) const;
	
	void TryAttack();
	void ReloadAct();
	void TryReload();
	void AttackAct();
	void AimingAct(bool IsTrigger);
	void FinishAttack();
	void FinishReload();
	FVector TraceHit() const;

public:	
	// Sets default values for this component's properties
	UUnrealCombatComponent();
	void SetTargetCC(ICharacterController* CC);
	void Swap();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
	virtual void EquipWeapon(AWeaponActor* Weapon) override;
	virtual void AimTrigger(bool IsTrigger) override;
	virtual void AttackTrigger(bool IsTrigger) override;
	virtual void Reload() override;
	
	UFUNCTION(Server, Reliable)
	void ServerAimTrigger(bool IsTrigger);
	UFUNCTION(Server, Reliable)
	void ServerAttackTrigger(bool IsTrigger);
	UFUNCTION(Server, Reliable)
	void ServerReload();
	
};
