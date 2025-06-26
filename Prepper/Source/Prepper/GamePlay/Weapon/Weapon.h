#pragma once

#include "WeaponTypes.h"
#include "Prepper/GamePlay/Equipment/Equipment.h"
#include "Weapon.generated.h"


struct FHUDPackage;
class IAmmoBox;
class ABCharacter;
class UWeaponMagazine;
class UWeaponAttacking;
class UWeaponTargeting;
class IWeaponHandler;
class UPlayerAimingEffect;
class ABasePlayerController;
class ABaseCharacter;
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
	TObjectPtr<UAnimationAsset> FireAnimation;
	UPROPERTY(EditAnywhere, Category ="Weapon Properties")
	TObjectPtr<USoundCue> EquipSound;
	UPROPERTY(EditAnywhere, Category = "Weapon Properties")
	float FireDelay = .15f;
	
	UPROPERTY(EditAnywhere)
	EWeaponType WeaponType;
	
	UPROPERTY(VisibleAnywhere, Category = "Weapon Noise")
	TObjectPtr<UPawnNoiseEmitterComponent> PawnNoiseEmitter; // 노이즈 발생 컴포넌트
	
	UPROPERTY(EditAnywhere, Category="Attack")
	TObjectPtr<USceneComponent> Muzzle;
	
	UPROPERTY(VisibleAnywhere, Category="Attack")
	TObjectPtr<UWeaponAttacking> Attacking;
	UPROPERTY(VisibleAnywhere, Category="Targeting")
	TObjectPtr<UWeaponTargeting> Targeting;
	UPROPERTY(VisibleAnywhere, Category="Magazine")
	TObjectPtr<UWeaponMagazine> Magazine;

	UPROPERTY(VisibleAnywhere, Category = "Mesh")
	TObjectPtr<UMeshComponent> WeaponMesh;
	UPROPERTY(VisibleAnywhere, Category = "Mesh")
	TObjectPtr<UStaticMeshComponent> StaticWeaponMesh;

	UPROPERTY(EditAnywhere, Category ="Equip")
	FName WeaponSocketName = FName("RightHandSocket");
	UPROPERTY(EditAnywhere, Category ="Equip")
	FName HolsteredWeaponSocketName = FName("HolsteredWeaponSocket");
	
	UPROPERTY()
	TArray<UPlayerAimingEffect*> AimingEffects;
	TArray<UPlayerAimingEffect*> GetAimingEffect();
	
	void WeaponPhysicsActive(bool bActive);
	
public:	
	AWeapon();
	
	virtual FString GetCode() override { return WeaponCode; }
	void PlayReload(TObjectPtr<ABCharacter> TargetCharacter, TObjectPtr<UAnimMontage> ReloadMontage) const;
	float GetFireDelay() const { return FireDelay; }
	FString GetAmmoValue();
	
	virtual void Interaction(APlayerCharacter* Target) override;
	virtual bool CanAttack();

	TArray<FVector_NetQuantize> GetTarget(FVector& HitTarget) const;

	virtual void GetCrosshair(float DeltaTime, bool bIsAiming, FHUDPackage& Crosshair);

	void SetStateAiming(TObjectPtr<ABCharacter> TargetCharacter);
	void SetStateUnAiming();
	
	void Fire(const TArray<FVector_NetQuantize>& HitTargets, AController* Attacker, bool IsSimulate) const;
	bool CanReload();
	virtual void OnEquipped(TObjectPtr<ABCharacter> TargetCharacter);
	virtual void OnDropped(TObjectPtr<ABCharacter> TargetCharacter);
	virtual void OnEquippedSecondary(TObjectPtr<ABCharacter> TargetCharacter);
	
	int GetLeftAmmo() const;
	void SetLeftAmmo(const int Amount) const;
	void Reload(IAmmoBox* AmmoBox);
	
	void PlayEquipWeaponSound(const AActor* TargetActor) const;

protected:
	virtual void BeginPlay() override;

	//Legacy
protected:
	UPROPERTY(EditAnywhere)
	float Damage = 20.f;

	UPROPERTY(ReplicatedUsing = OnRep_WeaponState, VisibleAnywhere, Category = "Weapon Properties")
	EWeaponState WeaponState;
	IWeaponHandler* GetWeaponHandler();
	
	UFUNCTION()
	void OnPingTooHigh(bool bPingTooHigh);
	
	UPROPERTY(Replicated, EditAnywhere)
	bool bUseServerSideRewind = false;
	
	UFUNCTION()
	void OnRep_WeaponState();
	
	UPROPERTY()
	ABaseCharacter* OwnerCharacter;
	UPROPERTY()
	ABasePlayerController* PlayerOwnerController;
	
	IWeaponHandler* WeaponHandler;

public:
	FORCEINLINE UMeshComponent* GetWeaponMesh()			const { return WeaponMesh; }
	
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
	EWeaponType GetWeaponType() const { return WeaponType; };
	virtual void OnWeaponStateSet();
	virtual void SetWeaponHandler(IWeaponHandler* NewOwner);
	virtual void SetWeaponState(EWeaponState State);
	
	void PlayReload(TObjectPtr<ABaseCharacter> TargetCharacter, TObjectPtr<UAnimMontage> ReloadMontage) const;
	virtual void Interaction(ICharacterController* Target) override;
	
	void SetStateAiming(TObjectPtr<APlayerCharacter> TargetCharacter);
	virtual void OnEquipped(ABaseCharacter* TargetCharacter);
	virtual void OnDropped(ABaseCharacter* TargetCharacter);
	virtual void OnEquippedSecondary(ABaseCharacter* TargetCharacter);
};