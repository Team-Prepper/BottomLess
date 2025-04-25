#include "RangeWeapon.h"
#include "Engine/SkeletalMeshSocket.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Prepper/__Legacy/Character/PlayerCharacter.h"

USkeletalMeshComponent* ARangeWeapon::GetRangeWeaponMesh()
{
	if(RangeWeaponMesh == nullptr)
	{
		RangeWeaponMesh = Cast<USkeletalMeshComponent>(WeaponMesh);
	}
	return RangeWeaponMesh;
}

void ARangeWeapon::SpendRound()
{
}

void ARangeWeapon::ClientUpdateAmmo_Implementation(int32 ServerAmmo)
{
	if (HasAuthority()) return;
 
	--Sequence;
}

void ARangeWeapon::AddAmmo(int32 AmmoToAdd)
{
	ClientAddAmmo(AmmoToAdd);
}

void ARangeWeapon::ClientAddAmmo_Implementation(int32 AmmoToAdd)
{
	if (HasAuthority()) return;
}

void ARangeWeapon::GetCrosshair(float DeltaTime, bool bIsAiming, FHUDPackage& Crosshair)
{
	Crosshair.CrosshairCenter = CrosshairCenter;
	Crosshair.CrosshairLeft   = CrosshairLeft;
	Crosshair.CrosshairRight  = CrosshairRight;
	Crosshair.CrosshairTop    = CrosshairTop;
	Crosshair.CrosshairBottom = CrosshairBottom;
	Crosshair.CrosshairSpread = 0.5f;

	if (bUseScatter || OwnerCharacter == nullptr) {
		return;
	}
	
	FVector2D WalkSpeedRange(0.f, OwnerCharacter->GetCharacterMovement()->MaxWalkSpeed);
	FVector2D VelocityMultiplierRange(0.f, 1.f);
	FVector Velocity = OwnerCharacter->GetVelocity();
	Velocity.Z = 0.f;

	const float CrosshairVelocityFactor =
		FMath::GetMappedRangeValueClamped(
			WalkSpeedRange,
			VelocityMultiplierRange,
			Velocity.Size());

	if (OwnerCharacter->GetCharacterMovement()->IsFalling())
	{
		CurCrosshairInAirFactor =
			FMath::FInterpTo(
				CurCrosshairInAirFactor,
				MaxCrosshairInAirFactor,
				DeltaTime, 2.25f);
	}
	else
	{
		CurCrosshairInAirFactor =
			FMath::FInterpTo(
				CurCrosshairInAirFactor,
				0,DeltaTime, 2.25f);
	}

	if (bIsAiming)
	{
		CurCrosshairAimFactor =
			FMath::FInterpTo(CurCrosshairAimFactor, MaxCrosshairAimFactor, DeltaTime, 30.f);
	}
	else
	{
		CurCrosshairAimFactor =
			FMath::FInterpTo(CurCrosshairAimFactor, 0.f, DeltaTime, 30.f);
	}

	CurCrosshairShootingFactor =
		FMath::FInterpTo(CurCrosshairShootingFactor, 0.f, DeltaTime, 40.f);

	Crosshair.CrosshairSpread += CrosshairVelocityFactor +
		CurCrosshairInAirFactor -
		CurCrosshairAimFactor +
		CurCrosshairShootingFactor;
	
}

FVector ARangeWeapon::TraceEndWithScatter(const FVector& HitTarget)
{
	const USkeletalMeshSocket* MuzzleSocket = GetRangeWeaponMesh()->GetSocketByName("Muzzle");
	if (MuzzleSocket == nullptr) return FVector();
	
	const FTransform SocketTransform = MuzzleSocket->GetSocketTransform(GetRangeWeaponMesh());
	const FVector TraceStart = SocketTransform.GetLocation();
	
	const FVector ToTargetNormalized = (HitTarget - TraceStart).GetSafeNormal();
	const FVector SphereCenter = TraceStart + ToTargetNormalized * DistanceToSphere;
	const FVector RandVec = UKismetMathLibrary::RandomUnitVector() * FMath::FRandRange(0.f, SphereRadius);
	const FVector EndLoc = SphereCenter + RandVec;
	const FVector ToEndLoc = EndLoc - TraceStart;

	return FVector(TraceStart + ToEndLoc * TRACE_LEN / ToEndLoc.Size());
}