#pragma once

#include "WeaponTypes.h"
#include "Prepper/__Legacy/Equipment/Equipment.h"
#include "Prepper/__Legacy/HUD/PrepperHUD.h"
#include "Prepper/__Legacy/Object/InteractableActor.h"
#include "WeaponActor.generated.h"


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
class PREPPER_API AWeaponActor : public AEquipment
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
	
	UPROPERTY(VisibleAnywhere, Category = "Weapon Noise")
	TObjectPtr<UPawnNoiseEmitterComponent> PawnNoiseEmitter; // 노이즈 발생 컴포넌트
	
	UPROPERTY(ReplicatedUsing = OnRep_WeaponState, VisibleAnywhere, Category = "Weapon Properties")
	EWeaponState WeaponState;
	
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
	
	void WeaponPhysicsActive(bool bActive);
	
public:	
	AWeaponActor();

	virtual TArray<UPlayerAimingEffect*> GetAimingEffect();

	virtual FString GetCode() override { return WeaponCode; }
	EWeaponType GetWeaponType() const { return WeaponType; };
	FName GetReloadActionName() const { return ReloadActionName; }
	float GetFireDelay() const { return FireDelay; };
	
	virtual void Interaction(APlayerCharacter* Target) override;
	virtual void Interaction(ICharacterController* Target) override;
	
	virtual bool CanAttack();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	TArray<FVector_NetQuantize> GetTarget(FVector& HitTarget) const;

	virtual void GetCrosshair(float DeltaTime, bool bIsAiming, FHUDPackage& Crosshair);
	
	virtual void OnEquipped(ABaseCharacter* TargetCharacter);
	virtual void OnDropped(ABaseCharacter* TargetCharacter);
	virtual void OnEquippedSecondary(ABaseCharacter* TargetCharacter);
	
	void Fire(const TArray<FVector_NetQuantize>& HitTargets, AController* Attacker, bool IsSimulate) const;
	virtual bool CanReload();
	virtual void OnEquipped(ABCharacter* TargetCharacter);
	virtual void OnDropped(ABCharacter* TargetCharacter);
	virtual void OnEquippedSecondary(ABCharacter* TargetCharacter);
	
	void PlayEquipWeaponSound(const AActor* TargetActor) const;

protected:
	virtual void BeginPlay() override;

	//Legacy
protected:
	UPROPERTY(EditAnywhere)
	float Damage = 20.f;

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

	UPROPERTY(EditAnywhere)
	EWeaponType WeaponType;

public:
	FORCEINLINE UMeshComponent* GetWeaponMesh()			const { return WeaponMesh; }
	
	int GetLeftAmmo() const;
	virtual void OnWeaponStateSet();
	virtual void SetWeaponHandler(IWeaponHandler* NewOwner);
	virtual void SetWeaponState(EWeaponState State);
};