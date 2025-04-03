// Fill out your copyright notice in the Description page of Project Settings.


#include "StatusEffectComponent.h"

#include "Kismet/GameplayStatics.h"
#include "Prepper/__Legacy/Character/PlayerCharacter.h"

UStatusEffectComponent::UStatusEffectComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	Character = nullptr;
}

void UStatusEffectComponent::BeginPlay()
{
	Super::BeginPlay();

	if (!Character) return;
}

void UStatusEffectComponent::Attach(IObserver<State>* Observer)
{
	Observers.insert(Observer);
	if (StateEffectMap.IsEmpty()) return;

	Observer->Update(
		State(FGaugeFloat(StateEffectMap[EStatusEffect::ESE_HUNGRY], 100),
		       FGaugeFloat(StateEffectMap[EStatusEffect::ESE_THIRSTY], 100),
		       FGaugeFloat(StateEffectMap[EStatusEffect::ESE_INFECTED], 100)));
}

void UStatusEffectComponent::Detach(IObserver<State>* Observer)
{
	Observers.erase(Observer);
}

void UStatusEffectComponent::Notify()
{
	if (StateEffectMap.IsEmpty()) return;

	const State Value(FGaugeFloat(StateEffectMap[EStatusEffect::ESE_HUNGRY], 100),
	                   FGaugeFloat(StateEffectMap[EStatusEffect::ESE_THIRSTY], 100),
	                   FGaugeFloat(StateEffectMap[EStatusEffect::ESE_INFECTED], 100));

	std::ranges::for_each(Observers, [&](IObserver<State>* Observer)
	{
		Observer->Update(Value);
	});
}

void UStatusEffectComponent::SetCharacter(ABaseCharacter* Target)
{
	Character = Target;
}

void UStatusEffectComponent::InitStateEffectMap()
{
	UE_LOG(LogTemp, Warning, TEXT("SET STATUS EFFECT LEVEL"));
	StateEffectMap.Emplace(EStatusEffect::ESE_HUNGRY, 100);
	StateEffectMap.Emplace(EStatusEffect::ESE_THIRSTY, 100);
	StateEffectMap.Emplace(EStatusEffect::ESE_INFECTED, 0);
}


void UStatusEffectComponent::TickComponent(float DeltaTime, ELevelTick TickType,
                                           FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void UStatusEffectComponent::StatusTimerStart()
{
	if (!Character || !Character->IsLocallyControlled()) return;

	UE_LOG(LogTemp, Warning, TEXT("StatusEffectReady"));
	StatusFlags.ClearAllEffects();
	InitStateEffectMap();

	GetWorld()->GetTimerManager().SetTimer(
		StatusTimerHandle,
		this,
		&UStatusEffectComponent::StatusTimerFinish,
		1.0f,
		true);

	Notify();
}

void UStatusEffectComponent::StatusTimerFinish()
{
	if (!Character) return;

	StateEffectMap[EStatusEffect::ESE_HUNGRY] -= StatusEffectTickValue[0];
	if (Character->GetMovementState() == EMovementState::EMS_Sprint ||
		Character->GetMovementState() == EMovementState::EMS_Aim)
	{
		StateEffectMap[EStatusEffect::ESE_THIRSTY] -= StatusEffectTickValue[1];
	}
	StateEffectMap[EStatusEffect::ESE_THIRSTY] -= StatusEffectTickValue[1];

	Notify();
	UpdateStatusEffect();
}

void UStatusEffectComponent::AddHungry(float Amount)
{
	StateEffectMap[EStatusEffect::ESE_HUNGRY] += Amount;
	Notify();
}

void UStatusEffectComponent::AddThirsty(float Amount)
{
	StateEffectMap[EStatusEffect::ESE_THIRSTY] += Amount;
	Notify();
}

void UStatusEffectComponent::UpdateStatusEffect()
{
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
			Character,
			EffectThresholds[0].DebuffValue,
			Character->GetController(),
			Character,
			UDamageType::StaticClass()
		);
	}

	if (StatusFlags.HasEffect(EStatusEffect::ESE_THIRSTY))
	{
		UE_LOG(LogTemp, Warning, TEXT("THIRSTY"));
		Character->CoefficientMovementSpeed = EffectThresholds[1].DebuffValue;
	}
	else
	{
		Character->CoefficientMovementSpeed = 1.0f;
	}

	/*
	for (const auto& EffectThreshold : EffectThresholds)
	{
		if (StatusFlags.HasEffect(EffectThreshold.Effect))
		{
			//UE_LOG(LogTemp, Log, TEXT("Current Status Effect: %s"), *EffectThreshold.EffectName);
		}
	}
	*/
}
