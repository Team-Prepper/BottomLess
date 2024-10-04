#include "MapInventory.h"
#include "Engine/GameInstance.h"
#include "UObject/ConstructorHelpers.h"

UMapInventory::UMapInventory()
{
	Owner = nullptr;
	BulletCount = 0;
}

bool UMapInventory::TryAddItem(const FString& ItemCode, const int Count)
{
	// 아이템이 존재한다면
	// 그 아이템의 소지수를 1 늘리고 true 반환
	if (ItemUnits.Contains(ItemCode))
	{
		const uint8 ItemCount = *ItemUnits.Find(ItemCode) + Count;
		ItemUnits.Add(ItemCode, ItemCount);
		
		UE_LOG(LogTemp, Warning, TEXT("Plus : Add Item %s + 1"), *ItemCode);
		Notify();
		return true;
	}

	// 새로운 아이템이 들어갈 공간이 없다면  false 반환
	if (ItemUnits.Num() >= InventorySize)
	{
		return false;
	}
	
	// 새로운 아이템을 추가하고 true 반환
	ItemUnits.Add(ItemCode, Count);
	UE_LOG(LogTemp, Warning, TEXT("Init : Add Item %s"), *ItemCode);

	ConvertMapToArray();
	Notify();
	return true;
}

bool UMapInventory::TryUseItem(const FString& ItemCode, const int Count)
{
	// 여기서는 아이템의 갯수만 제어
	// 효과 적용은 해당 아이템에서 직접 적용
	// 아이템이 충분히 존재하지 않는다면 return false
	if (!ItemUnits.Contains(ItemCode))
	{
		UE_LOG(LogTemp, Warning, TEXT("UseItem : No Item %s"), *ItemCode);
		return false;
	}
	const int32 ItemCount = *ItemUnits.Find(ItemCode) - Count;

	if (ItemCount < 0) return false;

	// 아이템 사용 후의 개수가 0인 경우 삭제
	if (ItemCount == 0)
	{
		ItemUnits.Remove(ItemCode);	
	}
	// 아닌 경우 아이템의 소지 수 -Count
	else
	{
		ItemUnits.Add(ItemCode, ItemCount);
	}
	
	UE_LOG(LogTemp, Warning, TEXT("Current Item :%s / Count : %d"), *ItemCode, ItemCount);
	ConvertMapToArray();
	Notify();
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

bool UMapInventory::CheckOwnItem(const FString& ItemCode)
{
	if (ItemUnits.Contains(ItemCode)) return true;
	return false;
}

void UMapInventory::QuickSlotAdd(const FString& ItemCode, const int Count, const int Idx)
{
	if (Idx >= MAX_QUICK_SLOT) return;
	
	QuickSlotItem[Idx] = ItemCode;
	QuickSlot.Add(ItemCode, Count);

	UE_LOG(LogTemp, Warning, TEXT("Add Item To QuickSlot:%s"), *ItemCode);
}

void UMapInventory::UseItemAtQuickSlot(const int Idx)
{
	TryUseItem(QuickSlotItem[Idx], 1);
}

TArray<IInventory::InventoryItem> UMapInventory::GetIter() const
{
	TArray<InventoryItem> Retval;
	for (auto Iter = ItemUnits.CreateConstIterator(); Iter; ++Iter)
	{
		Retval.Add(InventoryItem(Iter.Key(), Iter.Value()));
	}
	return Retval;
	
}

TArray<IInventory::InventoryItem> UMapInventory::GetQuickSlotIter() const
{
	TArray<InventoryItem> Retval;
	for (auto Iter = QuickSlot.CreateConstIterator(); Iter; ++Iter)
	{
		Retval.Add(InventoryItem(Iter.Key(), Iter.Value()));
	}
	return Retval;
}

// OBSERVER
void UMapInventory::Attach(IObserver<IInventory*>* Observer)
{
	Observers.insert(Observer);
	Observer->Update(this);
}

void UMapInventory::Detach(IObserver<IInventory*>* Observer)
{
	Observers.erase(Observer);
}

void UMapInventory::Notify()
{
	std::ranges::for_each(Observers, [&](IObserver<IInventory*>* Observer) {
		Observer->Update(this);
	});
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
		ItemUnits.Add(Elem.ItemCode, Elem.Count);
	}
}

void UMapInventory::OnRep_ItemUnits()
{
	ConvertArrayToMap(); // 클라이언트에서 배열을 맵으로 변환
	Notify(); // 옵저버 패턴으로 변경 사항을 알림
}

void UMapInventory::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UMapInventory, ReplicatedItemUnits);
}



