// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "../WeaponActor.h"
#include "RangeWeapon.generated.h"

UENUM(BlueprintType)
enum class EFireType : uint8
{
	EFT_HitScan UMETA(DisplayName = "Hit Scan Weapon"),
	EFT_Projectile UMETA(DisplayName = "Projectile Weapon"),
	EFT_Shotgun UMETA(DisplayName = "Shotgun Weapon"),

	EFT_MAX UMETA(DisplayName = "DefaultMAX")
};

UCLASS()
class PREPPER_API ARangeWeapon : public AWeaponActor
{
	GENERATED_BODY()

public:
	virtual void GetCrosshair(
		float DeltaTime, bool bIsAiming,
		FHUDPackage& Crosshair) override;
	/* CROSS HAIRS - 조준선 */
	UPROPERTY(EditAnywhere, Category = Crosshiar)
	UTexture2D* CrosshairCenter;

	UPROPERTY(EditAnywhere, Category = Crosshiar)
	UTexture2D* CrosshairLeft;
	UPROPERTY(EditAnywhere, Category = Crosshiar)
	UTexture2D* CrosshairRight;
	UPROPERTY(EditAnywhere, Category = Crosshiar)
	UTexture2D* CrosshairTop;
	UPROPERTY(EditAnywhere, Category = Crosshiar)
	UTexture2D* CrosshairBottom;

	UPROPERTY(EditAnywhere, Category = Crosshiar)
	float MaxCrosshairInAirFactor = 2.25f;
	UPROPERTY(EditAnywhere, Category = Crosshiar)
	float MaxCrosshairAimFactor = 0.58f;
	UPROPERTY(EditAnywhere, Category = Crosshiar)
	float MaxCrosshairShootingFactor = .75f;
	
	float CurCrosshairInAirFactor;
	float CurCrosshairAimFactor;
	float CurCrosshairShootingFactor;
protected:
	/* Zoom In - 줌 인 */
	UPROPERTY(EditAnywhere)
	float ZoomFOV = 30.f;
	
	UPROPERTY(EditAnywhere)
	float ZoomInterpSpeed = 20.f;

	/* Scatter - 산탄 */ 
	UPROPERTY(EditAnywhere, Category="Weapon Properties")
	float DistanceToSphere = 800.f;
	
	UPROPERTY(EditAnywhere, Category="Weapon Properties")
	bool bUseScatter = false;

	UPROPERTY(EditAnywhere, Category="Weapon Properties")
	float SphereRadius = 75.f;
	
	FVector TraceEndWithScatter(const FVector& HitTarget);

public:
	UPROPERTY(EditAnywhere)
	EFireType FireType;
	
	/* Ammo - 탄 */
	UPROPERTY(EditAnywhere, Category="Weapon Properties")
	bool bAutomatic = false;
	UPROPERTY(EditAnywhere, Category="Weapon Properties")
	bool bAutoReload = false;

	void AddAmmo(int32 AmmoToAdd);
	
protected:
	float TargetDistance;
	void SpendRound();
	
private:

	UFUNCTION(Client, Reliable)
	void ClientUpdateAmmo(int32 ServerAmmo);

	UFUNCTION(Client, Reliable)
	void ClientAddAmmo(int32 AmmoToAdd);

	int32 Sequence = 0;

	UPROPERTY(EditAnywhere)
	int32 MagCapacity;
	
	UPROPERTY()
	USkeletalMeshComponent* RangeWeaponMesh;

public:
	virtual bool CanReload() override;
	USkeletalMeshComponent* GetRangeWeaponMesh();
	FORCEINLINE int32 GetMagCapacity()					const { return MagCapacity; }
	FORCEINLINE float GetZoomedFOV()					const { return ZoomFOV; }
	FORCEINLINE float GetZoomedInterpSpeed()			const { return ZoomInterpSpeed; }
};
