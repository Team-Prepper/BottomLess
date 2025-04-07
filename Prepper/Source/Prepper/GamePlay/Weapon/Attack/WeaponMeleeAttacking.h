// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "WeaponAttacking.h"
#include "WeaponMeleeAttacking.generated.h"

class UNiagaraSystem;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class PREPPER_API UWeaponMeleeAttacking : public UWeaponAttacking
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category="Attack")
	float AttackRange = 50.f;
	UPROPERTY(EditAnywhere, Category="Attack")
	float AttackReach = 100.f;
	UPROPERTY(EditAnywhere, Category="Attack")
	float Damage;
	
	UPROPERTY(EditAnywhere, Category="Attack")
	TObjectPtr<UNiagaraSystem> ImpactParticles;

	UPROPERTY(EditAnywhere, Category="Attack")
	TObjectPtr<USoundCue> HitSound;

public:
	// Sets default values for this component's properties
	UWeaponMeleeAttacking();
	virtual void Fire (FVector Muzzle,
		const TArray<FVector_NetQuantize>& HitTargets, AController* Attacker, bool IsSimulate) override;
	void DamageTarget(const FHitResult& HitTarget, AController* Attacker, bool IsSimulate) const;
};
