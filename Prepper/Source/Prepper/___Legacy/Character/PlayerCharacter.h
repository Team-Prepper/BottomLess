#pragma once

#include "CoreMinimal.h"
#include "BaseCharacter.h"
#include "PetCharacter.h"
#include "Enums/CombatState.h"
#include "Prepper/GamePlay/PlayerAbility.h"
#include "Prepper/GamePlay/Character/Component/CustomCameraComponent.h"
#include "Prepper/___Legacy/ControlMapper/CharacterControlMapper.h"
#include "Prepper/___Legacy/ControlMapper/Controllable.h"
#include "Prepper/___Legacy/Interfaces/InteractWithCrosshairInterface.h"
#include "PlayerCharacter.generated.h"

class UInventoryComponent;
class AEquipment;
class UWidgetComponent;
class UInteractionComponent;
class UCombatComponent;
class APrepperHUD;
class UFlexibleSpringArmComponent;
class UInputAction;
class UCustomCameraComponent;
class UUnrealInventoryComponent;
class AWeapon;
class UStatusEffectComponent;
struct FInputActionValue;
enum class ETurningInPlace : uint8;

UCLASS()
class PREPPER_API APlayerCharacter : public ABaseCharacter,
									 public IInteractWithCrosshairInterface,
									 public IControllable, public IPlayerAbility
{
	GENERATED_BODY()

// Actor
public:
	APlayerCharacter();
	virtual void Tick(float DeltaTime) override;
	virtual void Destroyed() override;

protected:
	virtual void BeginPlay() override;
	
// BaseCharacter
public:
	UPROPERTY(Replicated)
	bool bDisableGamePlay = false;
public:
	virtual void Crouch(bool bClientSimulation = false) override;
	virtual void UnCrouch(bool bClientSimulation = false) override;
	
	virtual void Elim() override;
	virtual void MulticastElimAction() override;
	
	virtual void ReceiveDamage(float Damage, AController* InstigatorController, AActor* DamageCauser) override;
	
	UFUNCTION(BlueprintImplementableEvent)
	void ShowSniperScopeWidget(bool bShowScope);
	
protected:
	virtual void PlayHitReactMontage() override;
public:
	/* 행동관련 */
	virtual void Jump() override;
// IPlayerAbility
private:
	UPROPERTY()
	TObjectPtr<UInventoryComponent> Inventory;
	
	UPROPERTY(ReplicatedUsing = OnRep_EquippedBackpack)
	TObjectPtr<AItemBackpack> EquippedBackpack;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	TObjectPtr<UStatusEffectComponent> StatusEffect;
	
public:
	virtual UInventoryComponent* GetInventory() const override;

	virtual void AddItem(const FString& ItemCode, int Count = 1) override;
	virtual void UseQuickSlotItem(int Idx) override;
	virtual void EquipWeapon(AWeapon* Weapon) override;
	virtual void EquipBackpack(AItemBackpack* BackpackToEquip) override;
	
	virtual void Heal(float Amount) override;
	virtual void Eat(float Amount) override;
	virtual void Drink(float Amount) override;

	TArray<FString> GetEquipmentCodes();

private:
	UFUNCTION()
	void OnRep_EquippedBackpack();
	UFUNCTION(NetMulticast, Reliable)
	void MulticastAddItem(const FString& ItemCode, int Count);
	
// IControllable
private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	UCombatComponent* Combat;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	UInteractionComponent* InteractionComponent;
	
	UPROPERTY(VisibleAnywhere, Category = Camera)
	UFlexibleSpringArmComponent* FlexibleCameraBoom;

	UPROPERTY(VisibleAnywhere, Category = Camera)
	UCustomCameraComponent* FollowCamera;
	
	UPROPERTY(EditAnywhere, Category=Control)
	TSubclassOf<UCharacterControlMapper> MapperClass;
	UPROPERTY()
	TObjectPtr<UCharacterControlMapper> CharacterControlMapper;
	
	float AO_Yaw;
	float InterpAO_Yaw;
	float AO_Pitch;
	FRotator StartingAimRotation;
	float TimeSinceLastMovementReplication;
	float Sensitivity = 1;
	
public:
	void SetSensitivity(float Value);
	virtual void Move(const FInputActionValue& Value) override;
	virtual void Look(const FInputActionValue& Value) override;
	virtual IControlMapper* GetControlMapper() override;

	virtual void Interaction();
	virtual void CrouchToggle();
	virtual void Reload() override;

	virtual void SeatToggle(const bool Seat) override;

	UFUNCTION(NetMulticast, Reliable)
	void MulticastToggleInventory();
	
	UFUNCTION(BlueprintCallable)
	virtual UCustomCameraComponent* GetFollowCamera() override;
	
private:	
	// Aim 연산
	void CalculateAO_Pitch();
	void AimOffset(float DeltaTime);

	ETurningInPlace TurningInPlace;
	void TurnInPlace(float DeltaTime);
	
	// 자연스러운 회전 - 멀티플레이 proxies
	void SimProxiesTurn();

	void RotateInPlace(float DeltaTime);

// ETC
protected:
	// init 되었는지 확인하고 init함 _ DeathMatch
	void PollInit();
	virtual void OnRep_ReplicatedMovement() override;

private:
	UPROPERTY(EditAnywhere, Category = Skin)
	UStaticMeshComponent* Hair;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	UWidgetComponent* OverheadWidget;

	void SetPlayerEquipmentHiddenInGame(bool visible);
	void SetEquipmentHidden(AActor* Target, bool visible);
	
	/* For Animation */
	UPROPERTY(EditAnywhere, Category = Combat)
	UAnimMontage* FireWeaponMontage;

	UPROPERTY(EditAnywhere, Category = Combat)
	UAnimMontage* MeleeWeaponMontage;
	
	UPROPERTY(EditAnywhere, Category = Combat)
	UAnimMontage* ReloadMontage;
	
	UPROPERTY(EditAnywhere, Category = Combat)
	UAnimMontage* SwapMontage;

	/* Player Movement State */
	bool bBeforeSeat;

	UPROPERTY(VisibleAnywhere, Category = "Player Noise")
	UPawnNoiseEmitterComponent* PawnNoiseEmitter; // 노이즈 발생 컴포넌트
public:
	
private:
	void HideCamIfCharacterClose();
	UPROPERTY(EditAnywhere)
	float CamThreshold = 200.f;

	bool bRotateRootBone;
	float TurnThreshold = 0.5f;
	FRotator ProxyRotationLastFrame;
	FRotator ProxyRotation;
	float ProxyYaw;
	
	float CalculateSpeed();

public:
	
	bool IsWeaponEquipped();
	bool IsAiming();
	bool IsLocallyReloading();
	
	AWeapon* GetEquippedWeapon();
	FVector GetHitTarget() const;
	ECombatState GetCombatState() const;

	FORCEINLINE float GetAO_Yaw() const { return AO_Yaw;}
	FORCEINLINE float GetAO_Pitch() const { return AO_Pitch;}
	FORCEINLINE ETurningInPlace GetTurningInPlace() const { return TurningInPlace;}
	FORCEINLINE bool ShouldRotateRootBone() const { return bRotateRootBone; }
	FORCEINLINE UBaseCombatComponent* GetCombatComponent() const { return CombatComp; }
	FORCEINLINE UStatusEffectComponent* GetStatusEffectComponent() const { return StatusEffect; }
	FORCEINLINE bool GetDisableGamePlay() const { return bDisableGamePlay; }




	// P(펫 소환)키 매핑을 위한 오버라이드 
protected:
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;
// 펫
private:
	UPROPERTY(EditDefaultsOnly, Category = "Pet")
	TSubclassOf<APetCharacter> PetClass;

	UPROPERTY()
	APetCharacter* Pet;

	void TogglePet();
	void SpawnPet();
};
