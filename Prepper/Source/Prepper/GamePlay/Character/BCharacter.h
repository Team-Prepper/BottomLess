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

enum class EWeaponType : uint8;
class UAmmoBoxComponent;
class UStatusComponent;
class UBCombatComponent;
class UUnrealCharacterMoveComponent;
class UUnrealInteractionComponent;
class UUnrealCombatComponent;
class UUnrealStatusComponent;
class ICombat;
class IStatus;
class UPawnSensingComponent;
class UCustomCameraComponent;
class UFlexibleSpringArmComponent;

UCLASS()
class PREPPER_API ABCharacter : public ACharacter, public ICharacterController, public IPlayerAbility, public IDamageable
{
	GENERATED_BODY()
	
	UPROPERTY(VisibleAnywhere, Category = Camera)
	TObjectPtr<UFlexibleSpringArmComponent> FlexibleCameraBoom;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	TObjectPtr<UStatusComponent> Status;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	TObjectPtr<UBCombatComponent> Combat;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	TObjectPtr<UAmmoBoxComponent> AmmoBox;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	TObjectPtr<UInventoryComponent> Inventory;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	TObjectPtr<UUnrealInteractionComponent> Interaction;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	TObjectPtr<UUnrealCharacterMoveComponent> CharacterMove;
	
	UPROPERTY(VisibleAnywhere, Category = Camera)
	TObjectPtr<UCustomCameraComponent> FollowCamera;
	UPROPERTY(VisibleAnywhere, Category = Camera)
	TObjectPtr<UPawnSensingComponent> PawnSensing;
	
	UPROPERTY(EditAnywhere, Category = "Player Default Team Idx")
	int TeamIdx = 1;

public:
	// Sets default values for this actor's properties
	ABCharacter();
	virtual TObjectPtr<UStatusComponent> GetStatus() override;
	virtual TObjectPtr<UBCombatComponent> GetCombat() override;
	virtual TObjectPtr<UUnrealCharacterMoveComponent> GetMove();
	virtual IAmmoBox* GetAmmoBox() override;
	void SetAmmoBox(TObjectPtr<UAmmoBoxComponent> NewAmmoBox);
	
	void PlayAnim(const FString& String);

	virtual void Boarding(TObjectPtr<ACarPawn> Vehicle) override;
	virtual void Move(const FInputActionValue& Value) override;
	virtual void Look(const FInputActionValue& Value) override;
	
	virtual void CrouchToggle() override;
	
	virtual void JumpTrigger(bool IsTrigger) override;
	virtual void SprintTrigger(bool IsTrigger) override;
	
	virtual void EquipButtonPressed() override;

	void SetMaxSpeed(float MaxSpeed);
	
	void PlayAnim(UAnimMontage* Montage, const FName& SectionName = "") const;
	void GetLookDirection(FVector& Start, FVector& Forward) const;
	void AttachActorAtSocket(FName SocketName, AActor* TargetActor) const;
	void SetEquippedWeaponType(EWeaponType WeaponType);
	
	virtual void AddItem(const FString& ItemCode, int Count) override;
	virtual void UseQuickSlotItem(int Idx) override;
	virtual void EquipWeapon(class AWeapon* Weapon) override;
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

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	virtual void Crouch(bool bClientSimulation = false) override;
	virtual void UnCrouch(bool bClientSimulation = false) override;

	void AimTrigger(bool IsTrigger);
	
};