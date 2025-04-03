#include "MapInventory.h"

#include "Engine/GameInstance.h"
#include "Prepper/__Legacy/Item/ItemManager.h"
#include "UObject/ConstructorHelpers.h"

#define DEFAULT_QUICK_SLOT_ITEM FString("")

UMapInventory::UMapInventory()
{
	Owner = nullptr;
	BulletCount = 0;

	for (int i = 0; i < MAX_QUICK_SLOT; i++)
	{
		QuickSlotItem[i] = DEFAULT_QUICK_SLOT_ITEM;
	}
}

bool UMapInventory::TryAddMapExist(TMap<FString, uint8>& Target, const FString& ItemCode, const int Count)
{
	if (!Target.Contains(ItemCode)) return false;
	
	const uint8 ItemCount = *Target.Find(ItemCode) + Count;
	Target.Add(ItemCode, ItemCount);
		
	UE_LOG(LogTemp, Warning, TEXT("Plus : Add Item %s + %d"), *ItemCode, Count);
	ConvertMapToArray();
	Notify();
	return true;
}

bool UMapInventory::TryUseMapExist(TMap<FString, uint8>& Target, const FString& ItemCode, const int Count)
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

void UMapInventory::SetOwner(IPlayerAbility* NewOwner)
{
	Owner = NewOwner;
}

bool UMapInventory::TryAddItem(const FString& ItemCode, const int Count)
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
	UE_LOG(LogTemp, Warning, TEXT("Init : Add Item %s, %d"), *ItemCode, Count);

	ConvertMapToArray();
	Notify();
	return true;
}

bool UMapInventory::TryUseItem(const FString& ItemCode, const int Count)
{
	// 여기서는 아이템의 갯수만 제어
	// 효과 적용은 해당 아이템에서 직접 적용
	// 아이템이 충분히 존재하지 않는다면 return false
	if (!TryUseMapExist(ItemUnits, ItemCode, Count)) return false;
	
	ConvertMapToArray();
	return true;
}

bool UMapInventory::TryDiscardItem(const FString& ItemCode, const int Count)
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
	UE_LOG(LogTemp, Warning, TEXT("Current Item :%s / Count : %d"), *ItemCode, ItemCount);
	// 아이템 사용에 성공했으므로 true 반환
	ConvertMapToArray();
	Notify();
	return true;
}

int UMapInventory::TryGetItemCount(const FString& ItemCode)
{
	if (!ItemUnits.Contains(ItemCode))
	{
		UE_LOG(LogTemp, Warning, TEXT("No Item %s"), *ItemCode);
		return 0;
	}
	return *ItemUnits.Find(ItemCode);
}

void UMapInventory::QuickSlotAdd(const FString& ItemCode, const int Idx = 0)
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

	UE_LOG(LogTemp, Warning, TEXT("Add Item To QuickSlot:%s"), *ItemCode);
}

void UMapInventory::QuickSlotRemove(const int Idx)
{
	if (QuickSlotItem[Idx].Compare(DEFAULT_QUICK_SLOT_ITEM) == 0) return;
	
	QuickSlotItem[Idx] = DEFAULT_QUICK_SLOT_ITEM;

	Notify();
}

void UMapInventory::UseItemAtQuickSlot(const int Idx)
{
	if (!TryUseItem(QuickSlotItem[Idx], 1)) return;
	
	FItem* Item = ItemManager::GetInstance()->GetItem(QuickSlotItem[Idx]);
	if (Item == nullptr) return;
	Item->Use(Owner);
}

TArray<FItemConvertData> UMapInventory::GetIter() const
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

TArray<FItemConvertData> UMapInventory::GetQuickSlotIter() const
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

// OBSERVER
void UMapInventory::Attach(IObserver<IInventory*>* Observer)
{
	Observers.Add(Observer);
	Observer->Update(this);
}

void UMapInventory::Detach(IObserver<IInventory*>* Observer)
{
	Observers.Remove(Observer);
}

void UMapInventory::Notify()
{
	for (auto Observer : Observers)
	{
		Observer->Update(this);
		
	}
}

void UMapInventory::ChangingInventory(const TObjectPtr<UMapInventory> NewInventory)
{
	for (int i = 0; i < MAX_QUICK_SLOT; i++)
	{
		NewInventory->QuickSlotItem[i] = QuickSlotItem[i];
		QuickSlotItem[i] = DEFAULT_QUICK_SLOT_ITEM;
	}
	for (auto Item : ItemUnits)
	{
		NewInventory->ItemUnits.Add(Item.Key, Item.Value);
	}
	for (auto Observer : Observers)
	{
		NewInventory->Observers.Add(Observer);
	}
	NewInventory->SetOwner(Owner);
	NewInventory->Notify();
	ItemUnits.Empty();
	Observers.Empty();
}

void UMapInventory::ConvertMapToArray()
{
	ReplicatedItemUnits.Empty();
	for (const auto& Elem : ItemUnits)
	{
		FItemConvertData ItemData;
		ItemData.ItemCode = Elem.Key;
		ItemData.Count = Elem.Value;
		ReplicatedItemUnits.Add(ItemData);
	}
}

void UMapInventory::ConvertArrayToMap()
{
	ItemUnits.Empty();
	for (const auto& Elem : ReplicatedItemUnits)
	{
		UE_LOG(LogTemp, Warning, TEXT("OnRep_ItemUnits: %s, %d"), *Elem.ItemCode, Elem.Count);
		ItemUnits.Add(Elem.ItemCode, Elem.Count);
	}
}

void UMapInventory::OnRep_ItemUnits()
{
	UE_LOG(LogTemp, Warning, TEXT("OnRep_ItemUnits"));
	ConvertArrayToMap(); // 클라이언트에서 배열을 맵으로 변환
	Notify(); // 옵저버 패턴으로 변경 사항을 알림
}

void UMapInventory::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UMapInventory, ReplicatedItemUnits);
}



