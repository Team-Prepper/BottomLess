#include "HitScanWeapon.h"
#include "NiagaraFunctionLibrary.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystemComponent.h"
#include "Prepper/__Legacy/Character/BaseCharacter.h"
#include "Prepper/__Legacy/Interfaces/Damageable.h"
#include "Sound/SoundCue.h"

void AHitScanWeapon::Fire(const TArray<FVector_NetQuantize>& HitTargets)
{
	Super::Fire(HitTargets);

	APawn* OwnerPawn = Cast<APawn>(GetOwner());
	if (OwnerPawn == nullptr) return;
	
	const USkeletalMeshSocket* MuzzleSocket = GetRangeWeaponMesh()->GetSocketByName("Muzzle");
	if (!MuzzleSocket) return;

	FireEffect();
	
	FVector HitTarget = HitTargets.Top();
	FTransform SocketTransform = MuzzleSocket->GetSocketTransform(GetRangeWeaponMesh());

	FHitResult FireHit;

	if (WeaponTraceHit(SocketTransform.GetLocation(), HitTarget, FireHit))
	{
		HitEffect(FireHit);
	}
	
	AController* InstigatorController = OwnerPawn->GetController();
	
	if (!HasAuthority() || !InstigatorController) return;

	if (IDamageable* DamagedTarget = Cast<IDamageable>(FireHit.GetActor()))
	{
		DamagedTarget->ReceiveDamage(Damage, InstigatorController, this);
	}
}

void AHitScanWeapon::FireEffect()
{
	MakeNoise(1, OwnerCharacter, FVector::ZeroVector);
	
	if(MuzzleFlash)
	{
		UNiagaraFunctionLibrary::SpawnSystemAttached(
				MuzzleFlash,
				WeaponMesh,
				FName("Muzzle"),
				FVector::ZeroVector,
				FRotator::ZeroRotator,
				EAttachLocation::KeepRelativeOffset,
				true,
				true
			);
	}
	if(FireSound)
	{
		UGameplayStatics::PlaySoundAtLocation(
			this,
			FireSound,
			GetActorLocation());
	}
}

void AHitScanWeapon::HitEffect(const FHitResult& FireHit)
{
	if(ImpactParticles)
	{
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(
			GetWorld(),
			ImpactParticles,
			FireHit.ImpactPoint,
			FireHit.ImpactNormal.Rotation()
		);
	}
	if(HitSound)
	{
		UGameplayStatics::PlaySoundAtLocation(
		this,
		HitSound,
		FireHit.ImpactPoint);
	}
}

bool AHitScanWeapon::WeaponTraceHit(const FVector& TraceStart, const FVector& HitTarget, FHitResult& OutHit)
{
	UWorld* World = GetWorld();
	if (!World) return false;
	
	bool IsBlock = false;
	FVector End = TraceStart + (HitTarget - TraceStart) * 1.25f;

	World->LineTraceSingleByChannel(
		OutHit,
		TraceStart,
		End,
		ECollisionChannel::ECC_Visibility
	);
	if (OutHit.bBlockingHit)
	{
		End = OutHit.ImpactPoint;
		IsBlock = true;
	}
	if (BeamParticles)
	{
		UParticleSystemComponent* Beam = UGameplayStatics::SpawnEmitterAtLocation(
			World,
			BeamParticles,
			TraceStart,
			FRotator::ZeroRotator,
			true
		);
		if (Beam)
		{
			Beam->SetVectorParameter(FName("Target"), End);
		}
	}
	return IsBlock;
}
