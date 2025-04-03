// Fill out your copyright notice in the Description page of Project Settings.


#include "ProjectileWeapon.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Prepper/__Legacy/Character/BaseCharacter.h"
#include "../Projectile/Projectile.h"

void AProjectileWeapon::Fire(const TArray<FVector_NetQuantize>& HitTargets)
{
	Super::Fire(HitTargets);

	if (!HasAuthority()) return;
	if (!ProjectileClass) return;
	
	const USkeletalMeshSocket* MuzzleFlashSocket = GetRangeWeaponMesh()->GetSocketByName(FName("Muzzle"));
	if (!MuzzleFlashSocket) return;
	
	UWorld* World = GetWorld();
	if (!World) return;
	
	MakeNoise(1, OwnerCharacter, FVector::ZeroVector);
	
	APawn* InstigatorPawn = Cast<APawn>(GetOwner());
	if (!InstigatorPawn) return;

	FVector HitTarget = HitTargets.Top();
	FTransform SocketTransform = MuzzleFlashSocket->GetSocketTransform(GetRangeWeaponMesh());
	FVector ToTarget = HitTarget - SocketTransform.GetLocation();
	FRotator TargetRotation = ToTarget.Rotation();
	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = GetOwner();
	SpawnParams.Instigator = InstigatorPawn;
	
	World->SpawnActor<AProjectile>(
		ProjectileClass,
		SocketTransform.GetLocation(),
		TargetRotation,
		SpawnParams
		);
}