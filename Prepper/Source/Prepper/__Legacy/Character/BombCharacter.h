// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Prepper/__Legacy/Object/DamageableObject.h"
#include "BombCharacter.generated.h"

class AElectricSwitch;

UCLASS()
class PREPPER_API ABombCharacter : public ADamageableObject
{
	GENERATED_BODY()
private:
	float TimeSend;
	
	UPROPERTY(EditAnywhere)
	float MaxTime = 10.f;

	UPROPERTY()
	TObjectPtr<AElectricSwitch> TargetSwitch;

	UPROPERTY()
	bool IsElim = false;
	
public:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	// Sets default values for this actor's properties
	ABombCharacter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	void Bomb();
	void SetElectricSwitch(TObjectPtr<AElectricSwitch> Switch);
	virtual void RemoveAction() override;
};
