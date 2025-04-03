// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Effect/ItemEffect.h"
#include "GameFramework/Actor.h"
#include "Prepper/__Legacy/Interfaces/PlayerAbility.h"
#include "Item.generated.h"

USTRUCT()
struct PREPPER_API FItem
{
	GENERATED_BODY()
private:
	TArray<IItemEffect *> ItemEffects;
	static IItemEffect* StringToItemEffect(const FString& Value);
	
public:
	FItem();
	FItem(const FString& ItemEffectCode);
	
	void Use(IPlayerAbility* Target);
};
