#pragma once
#include "Prepper/GamePlay/CharacterController/CharacterController.h"
#include "Prepper/__Legacy/Equipment/Equipment.h"

#include "Weapon.generated.h"

class UWeaponTargeting;
class UPlayerAimingEffect;
class USoundCue;

UCLASS()
class PREPPER_API AWeapon : public AEquipment
{
	GENERATED_BODY()
	
protected:
	UPROPERTY(EditAnywhere,Category = "Weapon Properties")
	FName ReloadActionName = FName("AssaultRifle");
	UPROPERTY(EditAnywhere, Category="Weapon Properties")
	FString WeaponCode;
	UPROPERTY(EditAnywhere, Category="Weapon Properties")
	TArray<TSubclassOf<UPlayerAimingEffect>> AimingEffectClasses;
	UPROPERTY(EditAnywhere, Category ="Weapon Properties")
	UAnimationAsset* FireAnimation;
	
	UPROPERTY(EditAnywhere, Category ="Equip")
	FName WeaponSocketName = FName("RightHandSocket");
	UPROPERTY(EditAnywhere, Category ="Equip")
	FName HolsteredWeaponSocketName = FName("HolsteredWeaponSocket");
	
	UPROPERTY()
	TObjectPtr<USoundCue> EquipSound;
	
	UPROPERTY(EditAnywhere)
	float Damage = 20.f;
	UPROPERTY(EditAnywhere, Category = Combat)
	float FireDelay = .15f;

	UPROPERTY(VisibleAnywhere, Category = "Mesh")
	UMeshComponent* WeaponMesh;
	UPROPERTY(VisibleAnywhere, Category = "Mesh")
	UStaticMeshComponent* StaticWeaponMesh;

	UPROPERTY(VisibleAnywhere, Category="Targeting")
	TObjectPtr<UWeaponTargeting> Targeting;

	UPROPERTY(VisibleAnywhere, Category = "Weapon Noise")
	UPawnNoiseEmitterComponent* PawnNoiseEmitter; // 노이즈 발생 컴포넌트
	
	UPROPERTY()
	TArray<UPlayerAimingEffect*> AimingEffects;
	TArray<UPlayerAimingEffect*> GetAimingEffect();
	
public:	
	AWeapon();
	
	FName AttachSocketName() const { return WeaponSocketName; }
	FName GetReloadActionName() const { return ReloadActionName; }
	virtual FString GetCode() override { return WeaponCode; }
	float GetFireDelay() const { return FireDelay; }
	
	virtual int GetLeftAmmo() { return -1; }
	virtual void Interaction(ICharacterController* Target) override;
	
	TArray<FVector_NetQuantize> GetTarget(FVector& HitTarget) const;
	void Fire(const TArray<FVector_NetQuantize>& HitTargets);
	
	virtual bool CanReload();
	virtual void GetCrosshair(
		float DeltaTime, bool bIsAiming,
		TObjectPtr<UTexture2D>& Center,
		TObjectPtr<UTexture2D>& Left,
		TObjectPtr<UTexture2D>& Right,
		TObjectPtr<UTexture2D>& Top,
		TObjectPtr<UTexture2D>& Bottom,
		float &Spread);
	
	void PlayEquipWeaponSound();

protected:
	virtual void BeginPlay() override;

public:
	virtual void OnWeaponStateSet();
	virtual void OnEquipped();
	virtual void OnDropped();
	virtual void OnEquippedSecondary();

private:
	void WeaponPhysicsActive(bool bActive);
	
};