// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "UnrealPlayerInputComponent.generated.h"


struct FInputActionValue;
class UInputMappingContext;
class UInputAction;
class ICharacterController;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class PREPPER_API UUnrealPlayerInputComponent : public UActorComponent
{
	GENERATED_BODY()
	
	ICharacterController* CC;

public:
	// Sets default values for this component's properties
	UUnrealPlayerInputComponent();

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = true))
	TObjectPtr<UInputMappingContext> PlayerMappingContext;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = true))
	TObjectPtr<UInputAction> MoveAction;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = true))
	TObjectPtr<UInputAction> JumpAction;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = true))
	TObjectPtr<UInputAction> LookAction;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = true))
	TObjectPtr<UInputAction> SprintAction;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = true))
	TObjectPtr<UInputAction> EquipAction;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = true))
	TObjectPtr<UInputAction> CrouchAction;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = true))
	TObjectPtr<UInputAction> ReloadAction;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = true))
	TObjectPtr<UInputAction> AimAction;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = true))
	TObjectPtr<UInputAction> FireAction;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = true))
	TObjectPtr<UInputAction> OpenInventory;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = true))
	TObjectPtr<UInputAction> OpenSetting;

	void Move(const FInputActionValue& Value);
	void Look(const FInputActionValue& Value);
	void JumpButtonPressed();
	void JumpButtonReleased();
	void CrouchButtonPressed();
	void ReloadButtonPressed();
	void AimButtonPressed();
	void AimButtonReleased();
	void AttackButtonPressed();
	void AttackButtonReleased();

	void TabButtonPressed();
	void TabButtonReleased();
	
	void SprintButtonPressed();
	void SprintButtonReleased();
	void EquipButtonPressed();

public:
	virtual void SetInput(ICharacterController* cc, UEnhancedInputComponent* Input);
	TObjectPtr<UInputMappingContext> GetMappingContext();
};
