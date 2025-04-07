// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "WeaponMagazine.h"
#include "UObject/Object.h"
#include "WeaponLimitMagazine.generated.h"

/**
 * 
 */
UCLASS()
class PREPPER_API UWeaponLimitMagazine : public UWeaponMagazine
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, Category="Magazine")
	int MaxAmmo;
	UPROPERTY(EditAnywhere, Category="Magazine")
	int CurAmmo;

public:
	UWeaponLimitMagazine();
	virtual int GetLeftAmmo() override;
	
	virtual bool CanAttack() override;
	virtual bool CanReload() override;
	
	virtual void Reload(int Amount) override;
	virtual void UseAmmo(int Amount) override;
	
	virtual FString ToString() override;
};
