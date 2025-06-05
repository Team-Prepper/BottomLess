#include "HitScanWeapon.h"
#include "NiagaraFunctionLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystemComponent.h"
#include "Prepper/___Legacy/Character/BaseCharacter.h"
#include "Sound/SoundCue.h"

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
