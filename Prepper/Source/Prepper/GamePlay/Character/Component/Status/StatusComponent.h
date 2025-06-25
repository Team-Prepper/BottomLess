// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Prepper/__Base/ObserverPattern/Subject.h"
#include "Prepper/___Legacy/Character/Enums/StatusEffect.h"
#include "StatusComponent.generated.h"

class UCharacterMoveComponent;
class UUnrealCharacterMoveComponent;
enum class EStatusEffect : uint8;
class State;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class PREPPER_API UStatusComponent : public UActorComponent, public ISubject<UStatusComponent>
{
	GENERATED_BODY()

	TObjectPtr<UCharacterMoveComponent> TargetMove;
	
	TMap<EStatusEffect, float> StateEffectMap;
	FTimerHandle StatusTimerHandle;
	FStatusEffect StatusFlags; // 현재 상태 이상 플래그

	struct StatusEffectThreshold
	{
		EStatusEffect Effect;
		float Threshold;
		FString EffectName;
		float DebuffValue;
	};
	
	// 상태 효과와 임계값 배열
	const StatusEffectThreshold EffectThresholds[3] = {
		{ EStatusEffect::ESE_HUNGRY, 20.0f, "Hungry", 0.3f }, 
		{ EStatusEffect::ESE_THIRSTY, 30.0f, "Thirsty", 0.3f},
		{ EStatusEffect::ESE_INFECTED, 20.0f, "Infected", 1.0f }, 
	};
	
	const float StatusEffectTickValue[3] = { 0.2f, 0.3f, 0.5f };
protected:
	
	UPROPERTY(EditAnywhere, Category = "Player Stats")
	float MaxHealth = 100.f;
	UPROPERTY(EditAnywhere, Category = "Player Stats")
	float CurrentHealth = 100.f;
	
public:
	// Sets default values for this component's properties
	UStatusComponent();
	
	virtual int GetMaxHealth() const { return MaxHealth; }
	virtual int GetCurHealth() const { return CurrentHealth; }
	virtual void TakeDamage(int Amount);

	void StatusTimerStart(TObjectPtr<UCharacterMoveComponent> Target);
	void StatusTimerFinish();
	
	State GetState();
	
	virtual void AddHP(float Amount);
	
	void AddHungry(float Amount);
	void AddThirsty(float Amount);

	float GetHungryRatio() const;
	float GetThirstyRatio() const;
	float GetInfectedRatio() const;
	
private:
	TSet<IObserver<UStatusComponent>*> Observers;
public:
	virtual void Attach(IObserver<UStatusComponent>* Observer) override;
	virtual void Detach(IObserver<UStatusComponent>* Observer) override;
	virtual void Notify();
};
