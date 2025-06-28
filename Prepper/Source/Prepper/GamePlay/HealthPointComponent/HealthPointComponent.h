// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Prepper/__Base/ObserverPattern/Subject.h"
#include "HealthPointComponent.generated.h"


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class PREPPER_API UHealthPointComponent : public UActorComponent, public ISubject<UHealthPointComponent>
{
	GENERATED_BODY()
protected:
	UPROPERTY(EditAnywhere, Category = "HealthPoint")
	float MaxHealth = 100.f;
	UPROPERTY(EditAnywhere, Category = "HealthPoint")
	float CurrentHealth = 100.f;
	
private:
	TSet<IObserver<UHealthPointComponent>*> Observers;
	
public:
	// Sets default values for this component's properties
	UHealthPointComponent();
	
	virtual int GetMaxHealth() const { return MaxHealth; }
	virtual int GetCurHealth() const { return CurrentHealth; }
	virtual void TakeDamage(int Amount);
	virtual void AddHP(float Amount);
	
	virtual void Attach(IObserver<UHealthPointComponent>* Observer) override;
	virtual void Detach(IObserver<UHealthPointComponent>* Observer) override;
	virtual void Notify();
};
