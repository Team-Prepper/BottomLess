#pragma once

#include "CoreMinimal.h"
#include "BaseCombatComponent.h"
#include "Components/ActorComponent.h"
#include "CombatComponent.generated.h"

// 아이템 코드와 수량을 저장할 구조체 이름을 FItemConvertData로 변경
USTRUCT(BlueprintType)
struct FWeaponConvertData
{
	GENERATED_BODY()

	UPROPERTY()
	EWeaponType WeaponType;

	UPROPERTY()
	uint8 Count;
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PREPPER_API UCombatComponent : public UBaseCombatComponent
{
	GENERATED_BODY()
public:
	friend class APlayerCharacter;
	friend class ASurvivorController;
// Actor
public:	
	UCombatComponent();
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
protected:
	virtual void BeginPlay() override;
	
	// Equip Weapon
private:
	UPROPERTY(ReplicatedUsing = OnRep_SecondaryWeapon)
	AWeapon* SecondaryWeapon;
	
	UPROPERTY(Replicated)
	class ARangeWeapon* EquippedRangeWeapon;

protected:
	virtual void ActionEnd() override;
	
public:
	virtual void EquipWeapon(AWeapon* WeaponToEquip) override;

private:
	void EquipPrimaryWeapon(AWeapon* WeaponToEquip);
	void EquipSecondaryWeapon(AWeapon* WeaponToEquip);
	virtual void DropEquippedWeapon() override;
	
	UFUNCTION()
	void SetWeaponType();

	virtual void OnRep_EquippedWeapon() override;
	
	UFUNCTION()
	void OnRep_SecondaryWeapon();
	
// Swap Weapon
private:
	UPROPERTY(EditAnywhere, Category = Combat)
	UAnimMontage* SwapMontage;
public:
	void SwapWeapons();
	
private:
	
	UFUNCTION(NetMulticast, Reliable)
	void MulticastSwapWeapon();
	
	UFUNCTION(BlueprintCallable)
	void FinishSwap();
	
	UFUNCTION(BlueprintCallable)
	void FinishSwapAttachWeapons();
	
// Fire
private:
	UPROPERTY(EditAnywhere, Category = Combat)
	UAnimMontage* FireWeaponMontage;

	UPROPERTY(EditAnywhere, Category = Combat)
	UAnimMontage* MeleeWeaponMontage;
	
	bool bFireButtonPressed;

public:
	virtual void Fire() override;

protected:
	virtual void LocalFireWeapon(const TArray<FVector_NetQuantize>& TraceHitTargets, const bool IsSimulate) const override;
	virtual void FinishFire() override;
	
// Ammo
private:
	// carried ammo for cur equip weapon
	UPROPERTY(ReplicatedUsing = OnRep_CarriedAmmo)
	int32 CarriedAmmo;

	UPROPERTY(EditAnywhere)
	TMap<EWeaponType, int32> CarriedAmmoMap;

	UPROPERTY(ReplicatedUsing = OnRep_Ammo)
	TArray<FWeaponConvertData> ReplicatedWeaponAmmoData;

	UFUNCTION()
	void OnRep_Ammo();

	void ConvertArrayToMap();
	void ConvertMapToArray();
	
protected:
	UPROPERTY(EditAnywhere)
	int32 MaxCarriedAmmo = 500;
	
	virtual FGaugeInt GetAmmoShow() override;

public:
	virtual void AddAmmo(EWeaponType WeaponType, int32 AmmoAmount) override;
	virtual int UseAmmo(EWeaponType WeaponType, int32 MaxUse) override;
// Reload
private:
	bool bLocallyReload = false;
	
public:
	virtual void Reload() override;
	
protected:
	virtual void FinishReload() override;

// Set Aiming
private:
	// Aiming FOV
	float DefaultFOV;
	
public:
	virtual void SetAiming(bool bIsAiming) override;

private:
	
	virtual void ServerSetAiming(bool bIsAiming) override;

// Set Crosshair
private:
	float CrosshairVelocityFactor;
	float CrosshairInAirFactor;
	float CrosshairAimFactor;
	float CrosshairShootingFactor;

	void SetHUDCrosshair(float DeltaTime, const FLinearColor& CrosshairColor);
	
// Targeting
protected:
	UPROPERTY()
	class APrepperHUD* HUD;
	
	void TraceUnderCrosshair(FHitResult& TraceHitResult);

public:
	virtual void TargetElim() override;

private:
	void UpdateCarriedAmmo();
	
	UFUNCTION()
	void OnRep_CarriedAmmo();

};