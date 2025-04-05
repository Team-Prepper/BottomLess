// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameFramework/Character.h"
#include "BCharacter.generated.h"

class UCustomCameraComponent;
class UFlexibleSpringArmComponent;

UCLASS()
class PREPPER_API ABCharacter : public ACharacter
{
	GENERATED_BODY()
	
private:
	UPROPERTY(VisibleAnywhere, Category = Camera)
	TObjectPtr<UFlexibleSpringArmComponent> FlexibleCameraBoom;

	UPROPERTY(VisibleAnywhere, Category = Camera)
	TObjectPtr<UCustomCameraComponent> FollowCamera;
	
	UPROPERTY(EditAnywhere, Category = "Player Movement Speed")
	float WalkSpeed = 600;
	UPROPERTY(EditAnywhere, Category = "Player Movement Speed")
	float SprintSpeed = 900;
	UPROPERTY(EditAnywhere, Category = "Player Movement Speed")
	float AimMovementSpeed = 400.f;
	
	bool IsSprint;
	bool IsAiming;

	float GetSpeed() const;

public:
	// Sets default values for this actor's properties
	ABCharacter();
	bool GetIsAiming() const { return IsAiming; }
	void GetLookDirection(FVector& Start, FVector& Forward);
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void Move(float X, float Z);
	void Look(float Yaw, float Pitch);

	virtual void Crouch(bool bClientSimulation = false) override;
	void UnCrouch(bool bClientSimulation = false);

	void SprintTrigger(bool IsTrigger);
	void AimTrigger(bool IsTrigger);
	
};
