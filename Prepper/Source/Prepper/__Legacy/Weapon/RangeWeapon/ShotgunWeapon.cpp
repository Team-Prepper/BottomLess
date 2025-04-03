// Fill out your copyright notice in the Description page of Project Settings.


#include "ShotgunWeapon.h"

#include "Engine/SkeletalMeshSocket.h"
#include "Kismet/KismetMathLibrary.h"
#include "Prepper/__Legacy/Character/PlayerCharacter.h"

void AShotgunWeapon::Fire(const TArray<FVector_NetQuantize>& HitTargets)
{
	ARangeWeapon::Fire(HitTargets);
	
	APawn* OwnerPawn = Cast<APawn>(GetOwner());
	if (OwnerPawn == nullptr) return;

	const USkeletalMeshSocket* MuzzleSocket = GetRangeWeaponMesh()->GetSocketByName("Muzzle");
	if (!MuzzleSocket) return;

	FireEffect();
	
	const FTransform SocketTransform = MuzzleSocket->GetSocketTransform(GetRangeWeaponMesh());
	const FVector Start = SocketTransform.GetLocation();

	// hit Character - number of hit
	TMap<IDamageable*, uint32> HitMap;
	
	for(FVector_NetQuantize HitTarget : HitTargets)
	{
		FHitResult FireHit;
		WeaponTraceHit(Start, HitTarget, FireHit);
		HitEffect(FireHit);

		if (!HasAuthority()) continue;
		
		IDamageable* DamagedTarget = Cast<IDamageable>(FireHit.GetActor());
		
		if (!DamagedTarget) continue;

		if (HitMap.Contains(DamagedTarget))
		{
			HitMap[DamagedTarget]++;
		}
		else
		{
			HitMap.Emplace(DamagedTarget, 1);
		}
	}
	
	AController* InstigatorController = OwnerPawn->GetController();
	
	if (!HasAuthority() || !InstigatorController) return;
	
	for (auto HitPair : HitMap)
	{
		if (!HitPair.Key) continue;
		
		HitPair.Key->ReceiveDamage(Damage * HitPair.Value, InstigatorController, this);
	}
	
}

void AShotgunWeapon::ShotgunTraceEndWithScatter(const FVector& HitTarget, TArray<FVector_NetQuantize>& HitTargets)
{
	const USkeletalMeshSocket* MuzzleSocket = GetRangeWeaponMesh()->GetSocketByName("Muzzle");
	if (MuzzleSocket == nullptr) return;

	const FTransform SocketTransform = MuzzleSocket->GetSocketTransform(GetRangeWeaponMesh());
	const FVector TraceStart = SocketTransform.GetLocation();

	const FVector ToTargetNormalized = (HitTarget - TraceStart).GetSafeNormal();
	const FVector SphereCenter = TraceStart + ToTargetNormalized * DistanceToSphere;

	for (uint32 i = 0; i < NumberOfPellets; i++)
	{
		const FVector RandVec = UKismetMathLibrary::RandomUnitVector() * FMath::FRandRange(0.f, SphereRadius);
		const FVector EndLoc = SphereCenter + RandVec;
		FVector ToEndLoc = EndLoc - TraceStart;
		ToEndLoc = TraceStart + ToEndLoc * TRACE_LEN / ToEndLoc.Size();

		HitTargets.Add(ToEndLoc);
	}
}
