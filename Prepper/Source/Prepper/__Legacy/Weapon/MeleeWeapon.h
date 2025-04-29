#pragma once

#include "CoreMinimal.h"
#include "Prepper/GamePlay/Weapon/Weapon.h"
#include "MeleeWeapon.generated.h"

class UNiagaraSystem;

UCLASS()
class PREPPER_API AMeleeWeapon : public AWeapon
{
	GENERATED_BODY()

public:
	AMeleeWeapon();

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
