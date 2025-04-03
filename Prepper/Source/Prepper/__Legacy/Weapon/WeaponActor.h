#pragma once

#include "WeaponTypes.h"
#include "Prepper/__Legacy/Equipment/Equipment.h"
#include "Prepper/__Legacy/Object/InteractableActor.h"
#include "WeaponActor.generated.h"


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
public:
	UPROPERTY(EditAnywhere,Category = "Weapon Properties")
	FName ReloadActionName = FName("AssaultRifle");
protected:
	UPROPERTY(EditAnywhere, Category="Weapon Properties")
	FString WeaponCode;
	UPROPERTY(EditAnywhere, Category="Weapon Properties")
	TArray<TSubclassOf<UPlayerAimingEffect>> AimingEffectClasses;
	UPROPERTY(EditAnywhere, Category ="Weapon Properties")
	UAnimationAsset* FireAnimation;
	UPROPERTY(ReplicatedUsing = OnRep_WeaponState, VisibleAnywhere, Category = "Weapon Properties")
	EWeaponState WeaponState;
	
	UPROPERTY(EditAnywhere, Category="Attack")
	TObjectPtr<USceneComponent> Muzzle;
	
	UPROPERTY(VisibleAnywhere, Category="Targeting")
	TObjectPtr<UWeaponTargeting> Targeting;
	UPROPERTY(VisibleAnywhere, Category="Attack")
	TObjectPtr<UWeaponAttacking> Attacking;

	UPROPERTY(VisibleAnywhere, Category = "Mesh")
	UMeshComponent* WeaponMesh;
	UPROPERTY(VisibleAnywhere, Category = "Mesh")
	UStaticMeshComponent* StaticWeaponMesh;

	UPROPERTY(EditAnywhere, Category ="Equip")
	FName WeaponSocketName = FName("RightHandSocket");
	UPROPERTY(EditAnywhere, Category ="Equip")
	FName HolsteredWeaponSocketName = FName("HolsteredWeaponSocket");
	
	UPROPERTY()
	TArray<UPlayerAimingEffect*> AimingEffects;
	
public:	
	AWeaponActor();

	virtual TArray<UPlayerAimingEffect*> GetAimingEffect();

	virtual FString GetCode() override { return WeaponCode; }
	virtual EWeaponType GetWeaponType() { return WeaponType; };
	virtual void SetWeaponHandler(IWeaponHandler* NewOwner);
	virtual void SetWeaponState(EWeaponState State);
	virtual int GetLeftAmmo() { return -1; }
	
	virtual void Interaction(APlayerCharacter* Target) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
	virtual void OnRep_Owner() override;

	TArray<FVector_NetQuantize> GetTarget(FVector& HitTarget);

	virtual void GetCrosshair(
		float DeltaTime, bool bIsAiming,
		TObjectPtr<UTexture2D>& Center,
		TObjectPtr<UTexture2D>& Left,
		TObjectPtr<UTexture2D>& Right,
		TObjectPtr<UTexture2D>& Top,
		TObjectPtr<UTexture2D>& Bottom,
		float &Spread);

	virtual FName AttachSocketName() { return WeaponSocketName; };
	virtual FName GetReloadActionName() { return ReloadActionName; };
	
	void Fire(const TArray<FVector_NetQuantize>& HitTargets, AController* Attacker);
	virtual void Fire(const TArray<FVector_NetQuantize>& HitTargets);
	virtual bool CanReload();
	
	UPROPERTY(EditAnywhere)
	float Damage = 20.f;

	UPROPERTY(EditAnywhere, Category = Combat)
	float FireDelay = .15f;

	virtual float GetFireDelay() { return FireDelay; };
	
	UPROPERTY()
	USoundCue* EquipSound;
	
	void PlayEquipWeaponSound();

protected:
	virtual void BeginPlay() override;

	virtual void OnWeaponStateSet();
	virtual void OnEquipped();
	virtual void OnDropped();
	virtual void OnEquippedSecondary();

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

private:
	void WeaponPhysicsActive(bool bActive);
	
public:
	FORCEINLINE UMeshComponent* GetWeaponMesh()			const { return WeaponMesh; }
	
	UPROPERTY(VisibleAnywhere, Category = "Weapon Noise")
	UPawnNoiseEmitterComponent* PawnNoiseEmitter; // 노이즈 발생 컴포넌트
};