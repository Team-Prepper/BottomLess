// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "NiagaraSystem.h"
#include "Prepper/GamePlay/Damageable.h"
#include "DamageableObject.generated.h"

class UHealthPointComponent;
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
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	TObjectPtr<UHealthPointComponent> HealthPoint;
	
public:	
	ADamageableObject();

	UFUNCTION()
	virtual void ReceiveDamage(float Damage, AController* InstigatorController, AActor* DamageCauser) override;
	virtual void Destroyed() override;

protected:
	virtual void BeginPlay() override;

	virtual void RemoveAction();

};
