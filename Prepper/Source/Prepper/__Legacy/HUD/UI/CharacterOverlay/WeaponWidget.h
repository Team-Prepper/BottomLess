// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Prepper/_Base/ObserverPattern/Observer.h"
#include "Prepper/_Base/Util/GaugeValue.h"
#include "WeaponWidget.generated.h"

class UImage;
class UTextBlock;
/**
 * 
 */
UCLASS()
class PREPPER_API UWeaponWidget : public UUserWidget, public IObserver<GaugeValue<int>>, public IObserver<FString>
{
	GENERATED_BODY()
private:
	UPROPERTY(EditAnywhere, meta = (BindWidget), Category=Weapon)
	TObjectPtr<UImage> WeaponIcon;
	UPROPERTY(EditAnywhere, meta = (BindWidget), Category=Weapon)
	TObjectPtr<UTextBlock> WeaponAmmoValue;
	UPROPERTY(EditAnywhere, meta = (BindWidget), Category=Weapon)
	TObjectPtr<UTextBlock> CarriedAmmoValue;
	
public:
	virtual void Update(const GaugeValue<int>& NewData) override;
	virtual void Update(const FString& NewData) override;
};
