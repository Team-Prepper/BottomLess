#include "HitScanWeapon.h"
#include "NiagaraFunctionLibrary.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystemComponent.h"
#include "Sound/SoundCue.h"

void AHitScanWeapon::Fire(const TArray<FVector_NetQuantize>& HitTargets)
{
	Super::Fire(HitTargets);

	FVector HitTarget = HitTargets.Top();
	APawn* OwnerPawn = Cast<APawn>(GetOwner());
	if (OwnerPawn == nullptr) return;
	AController* InstigatorController = OwnerPawn->GetController();

	MakeNoise(1, PlayerOwnerCharacter, FVector::ZeroVector);
	
	const USkeletalMeshSocket* MuzzleSocket = GetRangeWeaponMesh()->GetSocketByName("Muzzle");
	if (MuzzleSocket)
	{
		FTransform SocketTransform = MuzzleSocket->GetSocketTransform(GetRangeWeaponMesh());
		FVector Start = SocketTransform.GetLocation();

		FHitResult FireHit;
		bool IsHit = WeaponTraceHit(Start, HitTarget, FireHit);
		
		IDamageable* DamagedTarget = Cast<IDamageable>(FireHit.GetActor());
		if (DamagedTarget && HasAuthority() && InstigatorController)
		{
			UGameplayStatics::ApplyDamage(
				FireHit.GetActor(),
				Damage,
				InstigatorController,
				this,
				UDamageType::StaticClass()
			);
		}
		if(ImpactParticles && IsHit)
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
}

bool AHitScanWeapon::WeaponTraceHit(const FVector& TraceStart, const FVector& HitTarget, FHitResult& OutHit)
{
	bool IsBlock = false;
	UWorld* World = GetWorld();
	if (World)
	{
		FVector End = TraceStart + (HitTarget - TraceStart) * 1.25f;

		World->LineTraceSingleByChannel(
			OutHit,
			TraceStart,
			End,
			ECollisionChannel::ECC_Visibility
		);
		FVector BeamEnd = End;
		if (OutHit.bBlockingHit)
		{
			BeamEnd = OutHit.ImpactPoint;
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
				Beam->SetVectorParameter(FName("Target"), BeamEnd);
			}
		}
	}
	return IsBlock;
}
