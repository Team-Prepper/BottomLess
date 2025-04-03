// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Prepper/__Legacy/Interfaces/Damageable.h"
#include "Sound/SoundCue.h"
#include "NiagaraSystem.h"
#include "DamageableObject.generated.h"

class UNiagaraSystem;

UCLASS()
class PREPPER_API ADamageableObject : public AActor, public IDamageable
{
	GENERATED_BODY()
	
public:	
	ADamageableObject();
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UFUNCTION()
	virtual void ReceiveDamage(float Damage, AController* InstigatorController, AActor* DamageCauser) override;

	virtual void Destroyed() override;

protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere)
	UStaticMeshComponent* ObjectMesh;

	/* 체력 관련 */
	UPROPERTY(EditAnywhere, Category = "Player Stats")
	float MaxHealth = 100.f;
	
	UPROPERTY(ReplicatedUsing = OnRep_Health, VisibleAnywhere, Category = "Player Stats")
	float CurrentHealth = 100.f;

	UFUNCTION()
	void OnRep_Health();

	UPROPERTY(EditAnywhere)
	UNiagaraSystem* ImpactParticles;

	UPROPERTY(EditAnywhere)
	USoundCue* ImpactSound;

	UPROPERTY(EditAnywhere)
	UNiagaraSystem* DestroyParticles;

	UPROPERTY(EditAnywhere)
	USoundCue* DestroySound;

	UFUNCTION(Server, Reliable)
	void ServerDestroyObject();
	
	virtual void RemoveAction();
	
	UFUNCTION(NetMulticast, Reliable)
	void MulticastDestroyObject();
};
