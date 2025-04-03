// Fill out your copyright notice in the Description page of Project Settings.


#include "WeaponScatterTargeting.h"
#include "Kismet/KismetMathLibrary.h"

// Sets default values for this component's properties
UWeaponScatterTargeting::UWeaponScatterTargeting()
{
	DistanceToSphere = 800.f;
	SphereRadius = 75.f;
	NumberOfPellets = 1;
}

TArray<FVector_NetQuantize> UWeaponScatterTargeting::GetTarget(FVector& HitTarget)
{
	TArray<FVector_NetQuantize> HitTargets;
	
	const FVector TraceStart = GetOwner()->GetActorLocation();
	
	const FVector ToTargetNormalized = (HitTarget - TraceStart).GetSafeNormal();
	const FVector SphereCenter = TraceStart + ToTargetNormalized * DistanceToSphere;
	
	for (uint32 i = 0; i < NumberOfPellets; i++)
	{
		const FVector RandVec = UKismetMathLibrary::RandomUnitVector() * FMath::FRandRange(0.f, SphereRadius);
		const FVector EndLoc = SphereCenter + RandVec;
		const FVector ToEndLoc = EndLoc - TraceStart;
		
		HitTargets.Add(FVector(TraceStart + ToEndLoc * TRACE_LEN / ToEndLoc.Size()));
	}
	
	return HitTargets;
	
}