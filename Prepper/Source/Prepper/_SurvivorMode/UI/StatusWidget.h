// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Prepper/GamePlay/Character/Component/Status/StatusComponent.h"
#include "Prepper/__Base/ObserverPattern/Observer.h"
#include "StatusWidget.generated.h"

class UStatusComponent;
class State;
class UProgressBar;
/**
 * 
 */

UCLASS()
class PREPPER_API UStatusWidget : public UUserWidget, public IObserver<State>, public IObserver<UStatusComponent>
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, meta = (BindWidget), Category=Story)
	TObjectPtr<UProgressBar> HungerBar;
	UPROPERTY(EditAnywhere, meta = (BindWidget), Category=Story)
	TObjectPtr<UProgressBar> ThirstBar;
	UPROPERTY(EditAnywhere, meta = (BindWidget), Category=Story)
	TObjectPtr<UProgressBar> InfectionBar;

public:
	virtual void Update(const State& NewData) override;
	virtual void Update(const UStatusComponent& NewData) override;
	
};
