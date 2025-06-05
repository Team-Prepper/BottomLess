// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Prepper/GamePlay/Weapon/AmmoBox.h"
#include "Prepper/___Legacy/Character/Component/Combat/CombatComponent.h"
#include "AmmoBoxComponent.generated.h"


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class PREPPER_API UAmmoBoxComponent : public UActorComponent, public IAmmoBox
{
	GENERATED_BODY()

protected:
	UPROPERTY(EditAnywhere, Category = "AmmoBox")
	TMap<EWeaponType, int32> CarriedAmmoMap;
	UPROPERTY(EditAnywhere, Category = "AmmoBox")
	int MaxCarriedAmmo = 500;

public:
	// Sets default values for this component's properties
	UAmmoBoxComponent();
	virtual void SetAmmoMap(const TArray<FWeaponConvertData>& AmmoMap);
	TMap<EWeaponType, int32> GetAmmoMap() const { return CarriedAmmoMap; }
	
	virtual void AddAmmo(EWeaponType WeaponType, int32 AmmoAmount) override;
	virtual int UseAmmo(EWeaponType WeaponType, int32 MaxUse) override;
};
