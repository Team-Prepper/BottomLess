// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <set>

#include "CoreMinimal.h"
#include "CharacterComponent.h"
#include "Status.h"
#include "Prepper/Enums/StatusEffect.h"
#include "Components/ActorComponent.h"
#include "Prepper/PlayerController/PrepperPlayerController.h"
#include "Prepper/_Base/ObserverPattern/Subject.h"
#include "StatusEffectComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PREPPER_API UStatusEffectComponent : public UActorComponent,
									public ISubject<Status>, public ICharacterComponent
{
	GENERATED_BODY()

public:	
	friend class APlayerCharacter;
	UStatusEffectComponent();
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	
protected:
	virtual void BeginPlay() override;

	// Observer Pattern
private:
	std::pmr::set<IObserver<Status>*> Observers;
	
public:
	virtual void Attach(IObserver<Status>* Observer) override;
	virtual void Detach(IObserver<Status>* Observer) override;
	virtual void Notify() override;
	
private:
	UPROPERTY()
	EStatusEffect CurrentStatusEffect;

	UPROPERTY()
	ABaseCharacter* Character;
	
	UPROPERTY()
	APrepperPlayerController* PrepperPlayerController;
public:
	virtual void SetCharacter(ABaseCharacter* Target) override;
	virtual void TargetElim() override {};

public:
	TMap<EStatusEffect, float> StateEffectMap;
	FStatusEffect StatusFlags; // 현재 상태 이상 플래그
	
	void InitStateEffectMap();

	struct StatusEffectThreshold
	{
		EStatusEffect Effect;
		float Threshold;
		FString EffectName;
		float DebuffValue;
	};

	const float StatusEffectTickValue[3] = { 0.2f, 0.3f, 0.5f };

	// 상태 효과와 임계값 배열
	const StatusEffectThreshold EffectThresholds[3] = {
		{ EStatusEffect::ESE_HUNGRY, 20.0f, "Hungry", 0.3f }, 
		{ EStatusEffect::ESE_THIRSTY, 30.0f, "Thirsty", 0.3f},
		{ EStatusEffect::ESE_INFECTED, 20.0f, "Infected", 1.0f }, 
	};
	
	void UpdateStatusEffect();
	void UpdateStatusEffectHUD();
	
	FTimerHandle StatusTimerHandle;
	void StatusTimerStart();
	void StatusTimerFinish();

	/*
	UFUNCTION()
	void ApplyStatusEffect();
	*/
};
