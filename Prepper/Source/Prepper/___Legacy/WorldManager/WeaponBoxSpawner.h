// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "WeaponBoxSpawner.generated.h"

UCLASS()
class PREPPER_API AWeaponBoxSpawner : public AActor
{
	GENERATED_BODY()
	
public:	
	AWeaponBoxSpawner();

	UPROPERTY(Replicated)
	class AInteractableBox* ControlledFallingActor;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UFUNCTION(NetMulticast, Reliable)
	void Multicast_DropActor();

	UFUNCTION(BlueprintCallable,Server,Reliable, Category="Spawning")
	void SpawnFallingActor_Server(FVector SpawnLocation);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Spawning")
	TSubclassOf<AInteractableBox> FallingActorClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Spawning")
	float SpawnHeight;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Falling")
	float FallingSpeed;

	// 블루프린트에서 범위 수정 가능하도록 UPROPERTY로 설정
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Spawning|Range")
	FVector MinSpawnRange = FVector(-1000.0f, -1000.0f, 0.0f);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Spawning|Range")
	FVector MaxSpawnRange = FVector(1000.0f, 1000.0f, 1000.0f);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Spawning|Range")
	float FirstTimeToSpawn = 120.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Spawning|Range")
	float SpawnRange = 20.f;
	
protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	void ExecutePeriodicFunction();

private:
	// 타이머 핸들
	FTimerHandle InitialDelayTimerHandle;
	FTimerHandle PeriodicTimerHandle;

	
};
