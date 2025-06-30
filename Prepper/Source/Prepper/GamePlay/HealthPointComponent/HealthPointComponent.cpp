// Fill out your copyright notice in the Description page of Project Settings.


#include "HealthPointComponent.h"

#include "Prepper/GamePlay/Damageable.h"


// Sets default values for this component's properties
UHealthPointComponent::UHealthPointComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;
	Target = nullptr;
	// ...
}

void UHealthPointComponent::SetDamageableTarget(IDamageable* DamageableTarget)
{
	Target = DamageableTarget;
}

void UHealthPointComponent::OnDeath()
{
	if (Target == nullptr) return;

	Target->OnDeath();
}

void UHealthPointComponent::TakeDamage(int Amount)
{
	CurrentHealth -= Amount;

	if (CurrentHealth < 0) CurrentHealth = 0;

	Notify();
	
	if(CurrentHealth > 0.0f) return;

	OnDeath();
}

void UHealthPointComponent::AddHP(float Amount)
{
	CurrentHealth += Amount;
	if (CurrentHealth > MaxHealth) CurrentHealth = MaxHealth;

	Notify();
	
}

void UHealthPointComponent::Attach(IObserver<UHealthPointComponent>* Observer)
{
	Observers.Add(Observer);;
	UE_LOG(LogTemp, Warning, TEXT("CurrentHealth: %d"), GetCurHealth());
	Observer->Update(*this);
}

void UHealthPointComponent::Detach(IObserver<UHealthPointComponent>* Observer)
{
	Observers.Remove(Observer);
}

void UHealthPointComponent::Notify()
{
	for(const auto Observer : Observers)
	{
		Observer->Update(*this);
	}
}
