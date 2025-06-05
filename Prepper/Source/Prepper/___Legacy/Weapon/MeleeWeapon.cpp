#include "MeleeWeapon.h"
#include "NiagaraFunctionLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundCue.h"


AMeleeWeapon::AMeleeWeapon()
{
	WeaponSocketName = FName("MeleeWeaponSocket");
}

void AMeleeWeapon::FindActorsWithinRadius()
{
	UWorld* World = Owner->GetWorld();
	if (!World)
	{
		UE_LOG(LogTemp, Warning, TEXT("Invalid World!"));
		return;
	}
	
	TArray<FHitResult> HitResults;
	FCollisionShape SphereCollisionShape = FCollisionShape::MakeSphere(AttackRange);
	FCollisionObjectQueryParams ObjectQueryParams = FCollisionObjectQueryParams(ECC_Pawn);

	ObjectQueryParams.AddObjectTypesToQuery(ECC_WorldStatic);
	ObjectQueryParams.AddObjectTypesToQuery(ECC_WorldDynamic);
	ObjectQueryParams.AddObjectTypesToQuery(ECC_Visibility);
	ObjectQueryParams.AddObjectTypesToQuery(ECC_PhysicsBody);
	
	GetWorld()->SweepMultiByObjectType(
        	HitResults,
        	Owner->GetActorLocation(),
        	Owner->GetActorLocation() + Owner->GetActorForwardVector() * AttackReach,
        	FQuat::Identity,
        	ObjectQueryParams,
        	SphereCollisionShape
        );
	
	
	for (const FHitResult& Hit : HitResults)
	{
		//CallDamageTargetAfterDelay(Hit); <- 현재 뭔가 오류 발생의 원인
		DamageTarget(Hit);
	}
}

// 애니메이션을 위해서 1초 뒤에 데미지 처리
void AMeleeWeapon::CallDamageTargetAfterDelay(const FHitResult& HitTarget)
{
	GetWorld()->GetTimerManager().SetTimer(TimerHandle, [this, HitTarget]()
	{
		DamageTarget(HitTarget);
	}, 1.0f, false);
}

void AMeleeWeapon::DamageTarget(const FHitResult& HitTarget)
{
	UE_LOG(LogTemp, Warning, TEXT("%s, %f"), *HitTarget.GetActor()->GetName(), Damage)
	
	APawn* OwnerPawn = Cast<APawn>(GetOwner());
	if (OwnerPawn == nullptr) return;
	if(!HitTarget.GetActor()) return;
	if(HitTarget.GetActor() == GetOwner()) return;

	
	AController* InstigatorController = OwnerPawn->GetController();
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
	if (HasAuthority() && InstigatorController)
	{
		DamagedTarget->ReceiveDamage(Damage, InstigatorController, this);
	}
}

