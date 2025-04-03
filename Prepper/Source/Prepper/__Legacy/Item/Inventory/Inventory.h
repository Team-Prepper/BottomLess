// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "Inventory.generated.h"
#define MAX_ITEM_COUNT 16

class IPlayerAbility;


// 아이템 코드와 수량을 저장할 구조체 이름을 FItemConvertData로 변경
USTRUCT(BlueprintType)
struct FItemConvertData
{
	GENERATED_BODY()

	UPROPERTY()
	FString ItemCode;

	UPROPERTY()
	uint8 Count;
};

UINTERFACE(MinimalAPI)
class UInventory : public UInterface
{
	GENERATED_BODY()
};

class PREPPER_API IInventory
{
	GENERATED_BODY()

public:
	virtual bool TryAddItem(const FString& ItemCode, int Count) PURE_VIRTUAL(IInventory::TryAddItem, return 0; ); 
	virtual bool TryUseItem(const FString& ItemCode, int Count) PURE_VIRTUAL(IInventory::TryUseItem, return 0; );
	virtual bool TryDiscardItem(const FString& ItemCode, int Count) PURE_VIRTUAL(IInventory::TryDiscardItem, return 0; );
	virtual void QuickSlotAdd(const FString& ItemCode, const int Idx) PURE_VIRTUAL();
	virtual void UseItemAtQuickSlot(const int Idx) PURE_VIRTUAL();
	virtual void QuickSlotRemove(const int Idx) PURE_VIRTUAL();

	virtual int TryGetItemCount(const FString& ItemCode) PURE_VIRTUAL(IInventory::TryGetItemCount, return 0; );

	virtual TArray<FItemConvertData> GetIter() const
	PURE_VIRTUAL(IInventory::GetIter, TArray<FItemConvertData> Retval; return Retval; );
	virtual TArray<FItemConvertData> GetQuickSlotIter() const
	PURE_VIRTUAL(IInventory::GetQuickSlotIter, TArray<FItemConvertData> Retval; return Retval; );

};
