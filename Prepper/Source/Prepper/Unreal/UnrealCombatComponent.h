// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Prepper/GamePlay/CharacterController/Combat.h"
#include "UnrealCombatComponent.generated.h"

class ICharacterController;
class AWeaponActor;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PREPPER_API UUnrealCombatComponent :
		public UActorComponent, public ICombat
{
	GENERATED_BODY()

	ICharacterController* TargetCC;
	
	UPROPERTY(ReplicatedUsing = OnRep_EquippedWeapon)
	TObjectPtr<AWeaponActor> EquippedWeapon;
	
	UPROPERTY(ReplicatedUsing=OnRep_Aiming)
	bool IsAiming;
	
	bool IsAttack;
	bool IsAimingLocal;

	UFUNCTION()
	void OnRep_Aiming();
	UFUNCTION()
	virtual void OnRep_EquippedWeapon();
	
	UFUNCTION(Server, Reliable)
	void ServerFireWeapon(const TArray<FVector_NetQuantize>& TraceHitTargets) const;
	UFUNCTION(NetMulticast, Reliable)
	void MulticastFireWeapon(const TArray<FVector_NetQuantize>& TraceHitTargets) const;
	
	
	void AimingAct(bool IsTrigger);
	void AttackAct(bool IsTrigger);
	FHitResult TraceHit();

public:	
	// Sets default values for this component's properties
	UUnrealCombatComponent();
	void SetTargetCC(ICharacterController* CC);

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
