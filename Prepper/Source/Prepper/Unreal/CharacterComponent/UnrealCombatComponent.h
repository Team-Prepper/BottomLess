// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Prepper/GamePlay/Character/Component/BCombatComponent.h"
#include "UnrealCombatComponent.generated.h"

class ABCharacter;
class ICharacterController;
class AWeapon;
class UAnimMontage;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PREPPER_API UUnrealCombatComponent : public UBCombatComponent
{
	GENERATED_BODY()
	
	UPROPERTY(ReplicatedUsing = OnRep_EquippedWeapon)
	TObjectPtr<AWeapon> NetworkEquippedWeapon;
	UPROPERTY(ReplicatedUsing = OnRep_SecondaryWeapon)
	TObjectPtr<AWeapon> NetworkSecondaryWeapon;
	UPROPERTY(ReplicatedUsing = OnRep_DroppedWeapon)
	TObjectPtr<AWeapon> NetworkDroppedWeapon;
	
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
	void OnRep_Ammo();
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
	void AimingAct(bool IsTrigger) const;
	void FinishAttack();
	void FinishReload();
	FVector TraceHit() const;

public:	
	// Sets default values for this component's properties
	UUnrealCombatComponent();
	virtual void Swap() override;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
	virtual void EquipWeapon(AWeapon* Weapon) override;
	virtual void AimTrigger(bool IsTrigger) override;
	virtual void AttackTrigger(bool IsTrigger) override;
	virtual void Reload() override;
	
	virtual FString GetAmmoValue() const override;
	
	UFUNCTION(Server, Reliable)
	void ServerAimTrigger(bool IsTrigger);
	UFUNCTION(Server, Reliable)
	void ServerAttackTrigger(bool IsTrigger);
	UFUNCTION(Server, Reliable)
	void ServerReload();
	
};
