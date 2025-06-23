// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "WheeledVehiclePawn.h"
#include "Prepper/GamePlay/Damageable.h"
#include "Prepper/GamePlay/Interactable.h"
#include "Car.generated.h"

class UPassengerComponent;
class UInputAction;
struct FInputActionValue;
class USphereComponent;
class UChaosWheeledVehicleMovementComponent;
class UCameraComponent;
class USpringArmComponent;
class APrepperGameMode;

DECLARE_LOG_CATEGORY_EXTERN(LogTemplateVehicle, Log, All);

UCLASS()
class PREPPER_API ACar : public AWheeledVehiclePawn, public IInteractable, public IDamageable
{
	GENERATED_BODY()
	
	/** Spring Arm for the front camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USpringArmComponent> FrontSpringArm;
	/** Front Camera component */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UCameraComponent> FrontCamera;

	/** Spring Arm for the back camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USpringArmComponent> BackSpringArm;	
	/** Back Camera component */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UCameraComponent> BackCamera;

	/** Cast pointer to the Chaos Vehicle movement component */
	TObjectPtr<UChaosWheeledVehicleMovementComponent> ChaosVehicleMovement;

	UPROPERTY(VisibleAnywhere, Category = "Trigger")
	TObjectPtr<USphereComponent> AreaSphere;
	UPROPERTY(VisibleAnywhere, Category = "Passenger")
	TObjectPtr<UPassengerComponent> Passenger;

public:
	virtual void Move(const FInputActionValue& Value, TObjectPtr<APawn> Attempter);
	virtual void Look(const FInputActionValue& Value);
	
	virtual void InteractionAct(ICharacterController* Target);
	virtual void ChangeCam();
	
	virtual void Interaction(APlayerCharacter* Target) override;
	virtual void Interaction(ICharacterController* Target) override;

	virtual void ShowPickUpWidget(bool bShowWidget) override;

protected:
	virtual void BeginPlay() override;

	/** Brake Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	UInputAction* BrakeAction;
	/** Handbrake Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	UInputAction* HandbrakeAction;
	/** Reset Vehicle Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	UInputAction* ResetVehicleAction;

	/** Keeps track of which camera is active */
	bool bFrontCameraActive = false;

	/* 체력 관련 */
	UPROPERTY(EditAnywhere, Category = "Player Stats")
	float MaxHealth = 100.f;
	
	UPROPERTY(VisibleAnywhere, Category = "Player Stats")
	float CurrentHealth = 100.f;

public:
	ACar();

	virtual void SetupPlayerInputComponent(UInputComponent* InputComponent) override;
	virtual void Tick(float Delta) override;

	UFUNCTION()
	virtual void ReceiveDamage(float Damage, AController* InstigatorController, AActor* DamageCauser) override;

protected:
	/** Handles brake start/stop inputs */
	void StartBrake(const FInputActionValue& Value);
	void StopBrake(const FInputActionValue& Value);

	/** Handles handbrake start/stop inputs */
	void StartHandbrake(const FInputActionValue& Value);
	void StopHandbrake(const FInputActionValue& Value);

	/** Handles reset vehicle input */
	void ResetVehicle(const FInputActionValue& Value);

	/** Called when the brake lights are turned on or off */
	UFUNCTION(BlueprintImplementableEvent, Category="Vehicle")
	void BrakeLights(bool bBraking);

public:
	/** Returns the front spring arm subobject */
	FORCEINLINE USpringArmComponent* GetFrontSpringArm() const { return FrontSpringArm; }
	/** Returns the front camera subobject */
	FORCEINLINE UCameraComponent* GetFrontCamera() const { return FrontCamera; }
	/** Returns the back spring arm subobject */
	FORCEINLINE USpringArmComponent* GetBackSpringArm() const { return BackSpringArm; }
	/** Returns the back camera subobject */
	FORCEINLINE UCameraComponent* GetBackCamera() const { return BackCamera; }
	/** Returns the cast Chaos Vehicle Movement subobject */
	FORCEINLINE const TObjectPtr<UChaosWheeledVehicleMovementComponent>& GetChaosVehicleMovement() const
	{
		return ChaosVehicleMovement;
	}
};
