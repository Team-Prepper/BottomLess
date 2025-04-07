// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "WeaponMagazine.generated.h"


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class PREPPER_API UWeaponMagazine : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UWeaponMagazine();
	virtual int GetLeftAmmo();
	
	virtual bool CanAttack();
	virtual bool CanReload();
	
	virtual void Reload(int Amount);
	virtual void UseAmmo(int Amount);
	virtual FString ToString();
	
};
