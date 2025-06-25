// Fill out your copyright notice in the Description page of Project Settings.


#include "StatusComponent.h"

#include "Kismet/GameplayStatics.h"
#include "Prepper/Unreal/CharacterComponent/UnrealCharacterMoveComponent.h"
#include "Prepper/__Base/Util/GaugeFloat.h"
#include "Prepper/___Legacy/Character/Component/State.h"


// Sets default values for this component's properties
UStatusComponent::UStatusComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	// ...
}

void UStatusComponent::TakeDamage(int Amount)
{
	CurrentHealth -= Amount;

	if (CurrentHealth < 0) CurrentHealth = 0;

	Notify();
	
	if(CurrentHealth != 0.f) return;
}

void UStatusComponent::StatusTimerStart(TObjectPtr<UCharacterMoveComponent> Target)
{
	TargetMove = Target;
	UE_LOG(LogTemp, Warning, TEXT("StatusEffectReady"));
	StatusFlags.ClearAllEffects();
	
	StateEffectMap.Emplace(EStatusEffect::ESE_HUNGRY, 100);
	StateEffectMap.Emplace(EStatusEffect::ESE_THIRSTY, 100);
	StateEffectMap.Emplace(EStatusEffect::ESE_INFECTED, 0);

	GetWorld()->GetTimerManager().SetTimer(
		StatusTimerHandle,
		this,
		&UStatusComponent::StatusTimerFinish,
		1.0f,
		true);

	Notify();
}

void UStatusComponent::StatusTimerFinish()
{

	StateEffectMap[EStatusEffect::ESE_HUNGRY] -= StatusEffectTickValue[0];
	StateEffectMap[EStatusEffect::ESE_THIRSTY] -= StatusEffectTickValue[1];
	
	if (TargetMove->IsSprint() || TargetMove->IsAiming())
	{
		StateEffectMap[EStatusEffect::ESE_THIRSTY] -= StatusEffectTickValue[1];
	}
	
	for (const auto& EffectThreshold : EffectThresholds)
	{
		const bool HasEffect = StatusFlags.HasEffect(EffectThreshold.Effect);
		const float EffectValue = StateEffectMap[EffectThreshold.Effect];

		if (!HasEffect && EffectValue <= EffectThreshold.Threshold)
		{
			StatusFlags.AddEffect(EffectThreshold.Effect);
		}
		else if (HasEffect && EffectValue > EffectThreshold.Threshold)
		{
			StatusFlags.RemoveEffect(EffectThreshold.Effect);
		}
	}

	if (StatusFlags.HasEffect(EStatusEffect::ESE_HUNGRY))
	{
		UE_LOG(LogTemp, Warning, TEXT("HUNGRY"));
		UGameplayStatics::ApplyDamage(
			GetOwner(),
			EffectThresholds[0].DebuffValue,
			GetOwner<AActor>()->GetInstigatorController(),
			GetOwner(),
			UDamageType::StaticClass()
		);
	}

	if (StatusFlags.HasEffect(EStatusEffect::ESE_THIRSTY))
	{
		UE_LOG(LogTemp, Warning, TEXT("THIRSTY"));
		TargetMove->CoefficientMovementSpeed = EffectThresholds[1].DebuffValue;
	}
	else
	{
		TargetMove->CoefficientMovementSpeed = 1.0f;
	}
}

State UStatusComponent::GetState()
{
	return State(FGaugeFloat(StateEffectMap[EStatusEffect::ESE_HUNGRY], 100),
					   FGaugeFloat(StateEffectMap[EStatusEffect::ESE_THIRSTY], 100),
					   FGaugeFloat(StateEffectMap[EStatusEffect::ESE_INFECTED], 100));
}

void UStatusComponent::AddHP(float Amount)
{
	CurrentHealth += Amount;
	if (CurrentHealth > MaxHealth) CurrentHealth = MaxHealth;

	Notify();
}

void UStatusComponent::AddHungry(float Amount)
{
	StateEffectMap[EStatusEffect::ESE_HUNGRY] += Amount;
	Notify();
}

void UStatusComponent::AddThirsty(float Amount)
{
	StateEffectMap[EStatusEffect::ESE_THIRSTY] += Amount;
	Notify();
}

float UStatusComponent::GetHungryRatio() const
{
	return FGaugeFloat(StateEffectMap[EStatusEffect::ESE_HUNGRY], 100).GetRatio();
}

float UStatusComponent::GetThirstyRatio() const
{
	return FGaugeFloat(StateEffectMap[EStatusEffect::ESE_THIRSTY], 100).GetRatio();
}

float UStatusComponent::GetInfectedRatio() const
{
	return FGaugeFloat(StateEffectMap[EStatusEffect::ESE_INFECTED], 100).GetRatio();
}

void UStatusComponent::Attach(IObserver<UStatusComponent>* Observer)
{
	Observers.Add(Observer);;
	UE_LOG(LogTemp, Warning, TEXT("CurrentHealth: %d"), GetCurHealth());
	Observer->Update(*this);
}

void UStatusComponent::Detach(IObserver<UStatusComponent>* Observer)
{
	Observers.Remove(Observer);
}

void UStatusComponent::Notify()
{
	for(const auto Observer : Observers)
	{
		Observer->Update(*this);
	}
}