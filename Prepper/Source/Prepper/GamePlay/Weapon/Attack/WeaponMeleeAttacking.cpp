// Fill out your copyright notice in the Description page of Project Settings.


#include "WeaponMeleeAttacking.h"

#include "NiagaraFunctionLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "Prepper/GamePlay/Damageable.h"
#include "Sound/SoundCue.h"


class IDamageable;
// Sets default values for this component's properties
UWeaponMeleeAttacking::UWeaponMeleeAttacking()
{
	PrimaryComponentTick.bCanEverTick = false;
	Damage = 20.f;

}

void UWeaponMeleeAttacking::Fire(FVector Muzzle,
	const TArray<FVector_NetQuantize>& HitTargets, AController* Attacker, bool IsSimulate)
{
	const TObjectPtr<UWorld> World = GetWorld();
	
	TArray<FHitResult> HitResults;
	FCollisionShape SphereCollisionShape = FCollisionShape::MakeSphere(AttackRange);
	FCollisionObjectQueryParams ObjectQueryParams = FCollisionObjectQueryParams(ECC_Pawn);

	ObjectQueryParams.AddObjectTypesToQuery(ECC_WorldStatic);
	ObjectQueryParams.AddObjectTypesToQuery(ECC_WorldDynamic);
	ObjectQueryParams.AddObjectTypesToQuery(ECC_Visibility);
	ObjectQueryParams.AddObjectTypesToQuery(ECC_PhysicsBody);
	
	GetWorld()->SweepMultiByObjectType(
			HitResults,
			Muzzle,
			Muzzle + GetOwner()->Owner->GetActorForwardVector() * AttackReach,
			FQuat::Identity,
			ObjectQueryParams,
			SphereCollisionShape
		);

	if (IsSimulate) return;
	
	for (const FHitResult& Hit : HitResults)
	{
		//CallDamageTargetAfterDelay(Hit); <- 현재 뭔가 오류 발생의 원인
		DamageTarget(Hit, Attacker, IsSimulate);
	}
}

void UWeaponMeleeAttacking::DamageTarget(const FHitResult& HitTarget, AController* Attacker, bool IsSimulate) const
{
	UE_LOG(LogTemp, Warning, TEXT("%s, %f"), *HitTarget.GetActor()->GetName(), Damage)

	if (GetOwner() == nullptr) return;
	if(!HitTarget.GetActor()) return;
	if(HitTarget.GetActor() == GetOwner()) return;

	IDamageable* DamagedTarget = Cast<IDamageable>(HitTarget.GetActor());
	
	if(!DamagedTarget) return;
	
	if(ImpactParticles)
	{
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(
			GetWorld(),
			ImpactParticles,
			HitTarget.GetActor()->GetActorLocation(),
			HitTarget.ImpactNormal.Rotation()
		);
	}
	if(HitSound)
	{
		UGameplayStatics::PlaySoundAtLocation(
		this,
		HitSound,
		HitTarget.ImpactPoint);
	}
	
	if (IsSimulate) return;
	if (!Attacker) return;

	DamagedTarget->ReceiveDamage(Damage, Attacker, GetOwner());
	
}

