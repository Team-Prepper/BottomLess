// Fill out your copyright notice in the Description page of Project Settings.


#include "StatusComponent.h"

#include "Kismet/GameplayStatics.h"
#include "Prepper/Unreal/CharacterComponent/UnrealCharacterMoveComponent.h"


// Sets default values for this component's properties
UStatusComponent::UStatusComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	// ...
}

void UStatusComponent::StatusTimerStart(const TObjectPtr<UCharacterMoveComponent> Target)
{
	TargetMove = Target;
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
	return StateEffectMap[EStatusEffect::ESE_HUNGRY] / 100.f;
}

float UStatusComponent::GetThirstyRatio() const
{
	return StateEffectMap[EStatusEffect::ESE_THIRSTY] / 100.f;
}

float UStatusComponent::GetInfectedRatio() const
{
	return StateEffectMap[EStatusEffect::ESE_INFECTED] / 100.f;
}

void UStatusComponent::Attach(IObserver<UStatusComponent>* Observer)
{
	Observers.Add(Observer);;
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