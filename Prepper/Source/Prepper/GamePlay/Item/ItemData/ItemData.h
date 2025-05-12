// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ItemMeta.h"
#include "GameFramework/Actor.h"
#include "Prepper/GamePlay/Item/Item.h"
#include "ItemData.generated.h"

USTRUCT(BlueprintType)
struct PREPPER_API FItemData : public FTableRowBase
{
	GENERATED_BODY()
	
public:
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ItemInfo")
	FString ItemCode;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ItemInfo")
	FString ItemEffect;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ItemInfo")
	FItemMeta ItemMeta;
	
	FItemData() : ItemCode(TEXT("DefaultString1")), ItemEffect(TEXT("DefaultString2")), ItemMeta(FItemMeta())
	{
	}
	
	FORCEINLINE FItem GetItem() const { return FItem(ItemEffect); }
	FORCEINLINE FItemMeta GetItemMeta() const { return ItemMeta; }
};
