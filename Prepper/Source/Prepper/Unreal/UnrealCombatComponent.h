// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Prepper/GamePlay/CharacterController/Combat.h"
#include "UnrealCombatComponent.generated.h"

class AWeaponActor;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PREPPER_API UUnrealCombatComponent :
		public UActorComponent, public ICombat
{
	GENERATED_BODY()
	
	UPROPERTY(ReplicatedUsing = OnRep_EquippedWeapon)
	AWeaponActor* EquippedWeapon;
	
	UPROPERTY(ReplicatedUsing=OnRep_Aiming)
	bool IsAiming;
	UPROPERTY(ReplicatedUsing=OnRep_Attack)
	bool IsAttack;
	
	bool IsAttackLocal;
	bool IsAimingLocal;

	UFUNCTION()
	void OnRep_Aiming();
	UFUNCTION()
	void OnRep_Attack();
	UFUNCTION()
	virtual void OnRep_EquippedWeapon();
	
	void AimingAct(bool IsTrigger);
	void AttackAct(bool IsTrigger);
	
public:	
	// Sets default values for this component's properties
	UUnrealCombatComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
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
