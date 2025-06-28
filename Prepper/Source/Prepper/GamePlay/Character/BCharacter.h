// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameFramework/Character.h"
#include "Prepper/GamePlay/Damageable.h"
#include "Prepper/GamePlay/PlayerAbility.h"
#include "Prepper/GamePlay/CharacterController/CharacterController.h"
#include "Prepper/GamePlay/Weapon/AmmoBox.h"
#include "BCharacter.generated.h"

class UHealthPointComponent;
enum class EWeaponType : uint8;

class ICombat;
class IStatus;

class UAmmoBoxComponent;
class UStatusComponent;
class UBCombatComponent;
class UCharacterMoveComponent;

class UInteractionHandlingComponent;

class UPawnSensingComponent;
class UCustomCameraComponent;
class UFlexibleSpringArmComponent;
class UElimDissolveComponent;

UCLASS()
class PREPPER_API ABCharacter : public ACharacter, public ICharacterController, public IPlayerAbility, public IDamageable
{
	GENERATED_BODY()
	
	UPROPERTY(VisibleAnywhere, Category = Camera)
	TObjectPtr<UFlexibleSpringArmComponent> FlexibleCameraBoom;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	TObjectPtr<UHealthPointComponent> HealthPoint;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	TObjectPtr<UStatusComponent> Status;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	TObjectPtr<UBCombatComponent> Combat;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	TObjectPtr<UAmmoBoxComponent> AmmoBox;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	TObjectPtr<UInventoryComponent> Inventory;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	TObjectPtr<UInteractionHandlingComponent> Interaction;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	TObjectPtr<UCharacterMoveComponent> CharacterMove;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	TObjectPtr<UElimDissolveComponent> ElimDissolve;
	
	UPROPERTY(VisibleAnywhere, Category = Camera)
	TObjectPtr<UCustomCameraComponent> FollowCamera;
	UPROPERTY(VisibleAnywhere, Category = Camera)
	TObjectPtr<UPawnSensingComponent> PawnSensing;
	
	UPROPERTY(EditAnywhere, Category = Anim)
	TObjectPtr<UAnimMontage> ElimMontage;
	
	UPROPERTY(EditAnywhere, Category = "Combat")
	TSubclassOf<AWeapon> WeaponActorClass;
	
	UPROPERTY(EditAnywhere, Category = "Player Default Team Idx")
	int TeamIdx = 1;

public:
	// Sets default values for this actor's properties
	ABCharacter();
	virtual TObjectPtr<APawn> GetPawn() override;
	virtual TObjectPtr<UHealthPointComponent> GetHealthPoint();
	virtual TObjectPtr<UStatusComponent> GetStatus() override;
	virtual TObjectPtr<UBCombatComponent> GetCombat() override;
	virtual TObjectPtr<UInventoryComponent> GetInventory() override;
	virtual TObjectPtr<UCharacterMoveComponent> GetMove();
	virtual IAmmoBox* GetAmmoBox() override;
	
	virtual void EquipWeapon(TObjectPtr<AWeapon> Weapon) override;
	void SetAmmoBox(TObjectPtr<UAmmoBoxComponent> NewAmmoBox);

	virtual void Boarding(TObjectPtr<ACar> Vehicle) override;
	virtual void GetOff() override;
	
	virtual void EquipBackpack(TObjectPtr<AItemBackpack> Backpack) override;
	
	virtual void Move(const FInputActionValue& Value) override;
	virtual void Look(const FInputActionValue& Value) override;
	
	virtual void CrouchToggle() override;
	
	virtual void JumpTrigger(bool IsTrigger) override;
	virtual void SprintTrigger(bool IsTrigger) override;
	
	virtual void EquipButtonPressed() override;

	void SetMaxSpeed(float MaxSpeed) const;
	
	void PlayAnim(UAnimMontage* Montage, const FName& SectionName = "") const;
	void GetLookDirection(FVector& Start, FVector& Forward) const;
	void AttachActorAtSocket(FName SocketName, AActor* TargetActor) const;
	void SetEquippedWeaponType(EWeaponType WeaponType);
	
	virtual void EquipBackpack(class AItemBackpack* BackpackToEquip) override;

	virtual void Heal(float Amount) override;
	virtual void Eat(float Amount) override;
	virtual void Drink(float Amount) override;

	virtual UInventoryComponent* GetInventory() const override;
	
	virtual void ReceiveDamage(float Damage, AController* InstigatorController, AActor* DamageCauser) override;
	void ElimCharacter();
	
	void SetTeamIdx(int Idx);
	int GetTeam() const { return TeamIdx; }
	TObjectPtr<UPawnSensingComponent> GetPawnSensing() const;
	TArray<FString> GetEquipmentCodes() const;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	void SpawnWeaponActor();

public:
	// Called every frame

	virtual void Crouch(bool bClientSimulation = false) override;
	virtual void UnCrouch(bool bClientSimulation = false) override;

	void AimTrigger(bool IsTrigger) const;
	
};