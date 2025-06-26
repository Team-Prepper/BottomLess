// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "NiagaraSystem.h"
#include "Prepper/GamePlay/Damageable.h"
#include "DamageableObject.generated.h"

class UNiagaraSystem;

UCLASS()
class PREPPER_API ADamageableObject : public AActor, public IDamageable
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere)
	TObjectPtr<UNiagaraSystem> ImpactParticles;
	UPROPERTY(EditAnywhere)
	TObjectPtr<USoundCue> ImpactSound;
	UPROPERTY(EditAnywhere)
	TObjectPtr<UNiagaraSystem> DestroyParticles;
	UPROPERTY(EditAnywhere)
	TObjectPtr<USoundCue> DestroySound;

	UPROPERTY(EditAnywhere)
	TObjectPtr<UStaticMeshComponent> ObjectMesh;

	/* 체력 관련 */
	UPROPERTY(EditAnywhere, Category = "Player Stats")
	float MaxHealth = 100.f;
	UPROPERTY(ReplicatedUsing = OnRep_Health, VisibleAnywhere, Category = "Player Stats")
	float CurrentHealth = 100.f;
	
public:	
	ADamageableObject();
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UFUNCTION()
	virtual void ReceiveDamage(float Damage, AController* InstigatorController, AActor* DamageCauser) override;
	virtual void Destroyed() override;

protected:
	virtual void BeginPlay() override;

	virtual void RemoveAction();
	UFUNCTION()
	void OnRep_Health();

};
