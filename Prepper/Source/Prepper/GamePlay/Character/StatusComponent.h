// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Prepper/_Base/ObserverPattern/Subject.h"
#include "StatusComponent.generated.h"


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class PREPPER_API UStatusComponent : public UActorComponent, public ISubject<UStatusComponent>
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UStatusComponent();
	
	virtual int GetMaxHealth() const { return 0; }
	virtual int GetCurHealth() const { return 0; }
	virtual void TakeDamage(int Amount) { }
	
private:
	TSet<IObserver<UStatusComponent>*> Observers;
public:
	virtual void Attach(IObserver<UStatusComponent>* Observer) override;
	virtual void Detach(IObserver<UStatusComponent>* Observer) override;
	virtual void Notify();
};
