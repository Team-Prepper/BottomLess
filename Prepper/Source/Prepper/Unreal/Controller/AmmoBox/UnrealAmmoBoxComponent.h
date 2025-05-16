// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Prepper/GamePlay/Character/Component/AmmoBoxComponent.h"
#include "UnrealAmmoBoxComponent.generated.h"


struct FWeaponConvertData;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class PREPPER_API UUnrealAmmoBoxComponent : public UAmmoBoxComponent
{
	GENERATED_BODY()

	UPROPERTY(ReplicatedUsing = OnRep_Ammo)
	TArray<FWeaponConvertData> ReplicatedWeaponAmmoData;
	
	UFUNCTION()
	void OnRep_Ammo();
	
	void ConvertArrayToMap();
	void ConvertMapToArray();
	
public:
	// Sets default values for this component's properties
	UUnrealAmmoBoxComponent();
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	virtual void SetAmmoMap(const TArray<FWeaponConvertData>& AmmoMap) override;
	virtual void AddAmmo(EWeaponType WeaponType, int32 AmmoAmount) override;
	virtual int UseAmmo(EWeaponType WeaponType, int32 MaxUse) override;
	
};