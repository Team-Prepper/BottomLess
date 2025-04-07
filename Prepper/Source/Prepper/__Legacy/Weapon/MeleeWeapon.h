#pragma once

#include "CoreMinimal.h"
#include "WeaponActor.h"
#include "MeleeWeapon.generated.h"

class UNiagaraSystem;

UCLASS()
class PREPPER_API AMeleeWeapon : public AWeaponActor
{
	GENERATED_BODY()

public:
	AMeleeWeapon();
	
	virtual bool CanReload() override { return false;}

	void FindActorsWithinRadius();
protected:
	UPROPERTY(EditAnywhere)
	UNiagaraSystem* ImpactParticles;

	UPROPERTY(EditAnywhere)
	USoundCue* HitSound;

	UPROPERTY(EditAnywhere)
	float AttackRange = 50.f;

	UPROPERTY(EditAnywhere)
	float AttackReach = 100.f;

private:
	FTimerHandle TimerHandle;
	
	void CallDamageTargetAfterDelay(const FHitResult& HitTarget);
	
	UFUNCTION()
	void DamageTarget(const FHitResult& HitTarget);
	
};
