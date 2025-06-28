// Fill out your copyright notice in the Description page of Project Settings.


#include "InventoryComponent.h"

#include "Prepper/GamePlay/Item/ItemManager.h"

bool UInventoryComponent::TryAddMapExist(TMap<FString, uint8>& Target, const FString& ItemCode, const int Count)
{
	if (!Target.Contains(ItemCode)) return false;
	
	const uint8 ItemCount = *Target.Find(ItemCode) + Count;
	Target.Add(ItemCode, ItemCount);
		
	UE_LOG(LogTemp, Warning, TEXT("Plus : Add Item %s + %d"), *ItemCode, Count);
	Notify();
	return true;
}

bool UInventoryComponent::TryUseMapExist(TMap<FString, uint8>& Target, const FString& ItemCode, const int Count)
{
	if (!Target.Contains(ItemCode)) return false;
	
	const int32 ItemCount = *Target.Find(ItemCode) - Count;

	if (ItemCount < 0) return false;
	
	UE_LOG(LogTemp, Warning, TEXT("Plus : Use Item %s - %d, %d"), *ItemCode, Count, ItemCount);

	// 아이템 사용 후의 개수가 0인 경우 삭제
	if (ItemCount == 0)
	{
		Target.Remove(ItemCode);	
	}
	// 아닌 경우 아이템의 소지 수 -Count
	else
	{
		Target.Add(ItemCode, ItemCount);
	}
	Notify();
	return true;
}

// Sets default values for this component's properties
UInventoryComponent::UInventoryComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;
	Owner = nullptr;
	// ...
}
void UInventoryComponent::SetOwner(IPlayerAbility* NewOwner)
{
	Owner = NewOwner;
}

void UInventoryComponent::ChangingInventory(TObjectPtr<UInventoryComponent> NewInventory)
{
}

bool UInventoryComponent::TryAddItem(const FString& ItemCode, const int Count)
{
	// 아이템이 존재한다면
	// 그 아이템의 소지수를 1 늘리고 true 반환
	if (TryAddMapExist(ItemUnits, ItemCode, Count))
	{
		return true;
	}

	// 새로운 아이템이 들어갈 공간이 없다면  false 반환
	if (ItemUnits.Num() >= InventorySize)
	{
		return false;
	}
	
	// 새로운 아이템을 추가하고 true 반환
	ItemUnits.Add(ItemCode, Count);
	
	Notify();
	return true;
}

bool UInventoryComponent::TryUseItem(const FString& ItemCode, const int Count)
{
	// 여기서는 아이템의 갯수만 제어
	// 효과 적용은 해당 아이템에서 직접 적용
	// 아이템이 충분히 존재하지 않는다면 return false
	if (!TryUseMapExist(ItemUnits, ItemCode, Count)) return false;
	
	Notify();
	return true;
}

bool UInventoryComponent::TryDiscardItem(const FString& ItemCode, const int Count)
{
	if (!ItemUnits.Contains(ItemCode))	return false;
	const int32 ItemCount = *ItemUnits.Find(ItemCode) - Count;
	if (ItemCount < 0) return false;
	if (ItemCount == 0)
	{
		ItemUnits.Remove(ItemCode);	
	}
	// 아닌 경우 아이템의 소지 수 -1
	else
	{
		ItemUnits.Add(ItemCode, ItemCount);
	}
	// 아이템 사용에 성공했으므로 true 반환
	Notify();
	return true;
}

int UInventoryComponent::TryGetItemCount(const FString& ItemCode)
{
	if (!ItemUnits.Contains(ItemCode))
	{
		UE_LOG(LogTemp, Warning, TEXT("No Item %s"), *ItemCode);
		return 0;
	}
	return *ItemUnits.Find(ItemCode);
}

void UInventoryComponent::QuickSlotAdd(const FString& ItemCode, const int Idx)
{
	if (Idx >= MAX_QUICK_SLOT) return;

	int TargetIdx = Idx;

	for (int i = 0; i < MAX_QUICK_SLOT; i++)
	{
		TargetIdx = Idx + i % MAX_QUICK_SLOT;
		if (!ItemUnits.Contains(QuickSlotItem[TargetIdx]) ||
			ItemUnits[QuickSlotItem[TargetIdx]] < 1) break;
	}
	
	QuickSlotItem[TargetIdx] = ItemCode;

	Notify();
}

void UInventoryComponent::QuickSlotRemove(const int Idx)
{
	if (QuickSlotItem[Idx].Compare(DEFAULT_QUICK_SLOT_ITEM) == 0) return;
	
	QuickSlotItem[Idx] = DEFAULT_QUICK_SLOT_ITEM;

	Notify();
}

void UInventoryComponent::UseItemAtQuickSlot(const int Idx)
{
	if (!TryUseItem(QuickSlotItem[Idx], 1)) return;
	
	FItem* Item = ItemManager::GetInstance()->GetItem(QuickSlotItem[Idx]);
	if (Item == nullptr) return;
	Item->Use(Owner);
}

TArray<FItemConvertData> UInventoryComponent::GetIter() const
{
	TArray<FItemConvertData> Retval;
	for (auto Iter = ItemUnits.CreateConstIterator(); Iter; ++Iter)
	{
		bool IsQuickSlotItem = false;
		for (int i = 0; i < MAX_QUICK_SLOT; i++)
		{
			if (Iter.Key().Compare(QuickSlotItem[i]) != 0) continue;
			IsQuickSlotItem = true;
			break;
		}
		if (IsQuickSlotItem) continue;
		Retval.Add(FItemConvertData(Iter.Key(), Iter.Value()));
	}
	return Retval;
	
}

TArray<FItemConvertData> UInventoryComponent::GetQuickSlotIter() const
{
	TArray<FItemConvertData> Retval;
	for (int i = 0; i < MAX_QUICK_SLOT; i++)
	{
		int Cnt = 0;
		if (ItemUnits.Contains(QuickSlotItem[i]))
		{
			Cnt = ItemUnits[QuickSlotItem[i]];
		}
		Retval.Add(FItemConvertData(QuickSlotItem[i], Cnt));
	}
	return Retval;
}

void UInventoryComponent::Attach(IObserver<UInventoryComponent*>* Observer)
{
	Observers.Add(Observer);
	Observer->Update(this);
}

void UInventoryComponent::Detach(IObserver<UInventoryComponent*>* Observer)
{
	Observers.Remove(Observer);
}

void UInventoryComponent::Notify()
{
	for (const auto Observer : Observers)
	{
		Observer->Update(this);
	}
}