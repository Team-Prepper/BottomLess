#include "MapInventory.h"
#include "Engine/GameInstance.h"
#include "ItemData/ItemManager.h"
#include "Prepper/GameInstance/PrepperGameInstance.h"
#include "Prepper/Interfaces/Inventory.h"
#include "UObject/ConstructorHelpers.h"

UMapInventory::UMapInventory()
{
	BulletCount = 0;
}

void UMapInventory::SetOwner(IPlayerAbility* Target)
{
	Owner = Target;
}

bool UMapInventory::TryAddItem(const FString& ItemCode)
{
	
	// 아이템이 존재한다면
	// 그 아이템의 소지수를 1 늘리고 true 반환
	if (ItemUnits.Contains(ItemCode))
	{
		const uint8 ItemCount = *ItemUnits.Find(ItemCode) + 1;
		ItemUnits.Add(ItemCode, ItemCount);
		
		UE_LOG(LogTemp, Warning, TEXT("Add Item %s + 1"), *ItemCode);
		return true;
	}

	// 새로운 아이템이 들어갈 공간이 없다면  false 반환
	if (ItemUnits.Num() >= MAX_ITEM_COUNT)
	{
		return false;
	}
	
	// 새로운 아이템을 추가하고 true 반환
	ItemUnits.Add(ItemCode, 1);
	UE_LOG(LogTemp, Warning, TEXT("Add Item %s"), *ItemCode);
	
	return true;
}

bool UMapInventory::TryUseItem(const FString& ItemCode)
{
	// 아이템이 존재하지 않는다면 return false
	if (!ItemUnits.Contains(ItemCode))	return false;
	UPrepperGameInstance* PrepperGameInstance = Cast<UPrepperGameInstance>(GetWorld()->GetGameInstance());
	const uint8 ItemCount = *ItemUnits.Find(ItemCode) - 1;
	
	PrepperGameInstance->ItemData.GetItem(ItemCode)->Use(Owner);

	// 아이템 사용 후의 개수가 0인 경우 삭제
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
	return true;
}

void UMapInventory::QuickSlotAdd(const FString& ItemCode, const int Idx)
{
	if (Idx >= MAX_QUICK_SLOT) return;
	QuickSlotItem[Idx] = ItemCode;

	UE_LOG(LogTemp, Warning, TEXT("Add Item To QuickSlot:%s"), *ItemCode);
}

void UMapInventory::UseItemAtQuickSlot(const int Idx)
{
	TryUseItem(QuickSlotItem[Idx]);
}

TArray<IInventory::InventoryItem> UMapInventory::GetIter()
{
	TArray<InventoryItem> Retval;
	for (auto Iter = ItemUnits.CreateConstIterator(); Iter; ++Iter)
	{
		
		Retval.Add(InventoryItem(Iter.Key(), Iter.Value()));
	}
	return Retval;
	
}

void UMapInventory::AddBullet(const uint8 Count)
{
	BulletCount += Count;
}

uint8 UMapInventory::GetBulletCount() const
{
	return BulletCount;
}


