#include "UnrealInventoryComponent.h"

#include "Engine/GameInstance.h"
#include "Net/UnrealNetwork.h"
#include "UObject/ConstructorHelpers.h"

UUnrealInventoryComponent::UUnrealInventoryComponent()
{
	Owner = nullptr;
	
	SetIsReplicated(true);

	for (int i = 0; i < MAX_QUICK_SLOT; i++)
	{
		QuickSlotItem[i] = DEFAULT_QUICK_SLOT_ITEM;
	}
}

void UUnrealInventoryComponent::ChangingInventory(const TObjectPtr<UInventoryComponent> NewInventory)
{
	for (auto Item : ItemUnits)
	{
		NewInventory->TryAddItem(Item.Key, Item.Value);
	}
	for (int i = 0; i < MAX_QUICK_SLOT; i++)
	{
		NewInventory->QuickSlotAdd(QuickSlotItem[i], i);
		QuickSlotItem[i] = DEFAULT_QUICK_SLOT_ITEM;
	}
	for (const auto Observer : Observers)
	{
		NewInventory->Attach(Observer);
	}
	NewInventory->SetOwner(Owner);
	NewInventory->Notify();
	ItemUnits.Empty();
	Observers.Empty();
}

bool UUnrealInventoryComponent::TryAddItem(const FString& ItemCode, const int Count)
{
	if (Super::TryAddItem(ItemCode, Count))
	{
		ConvertMapToArray();
		return true;
	}
	return false;
	
}

bool UUnrealInventoryComponent::TryUseItem(const FString& ItemCode, const int Count)
{
	if (Super::TryUseItem(ItemCode, Count))
	{
		ConvertMapToArray();
		return true;
	}

	return false;
}

bool UUnrealInventoryComponent::TryDiscardItem(const FString& ItemCode, const int Count)
{
	if (Super::TryDiscardItem(ItemCode, Count))
	{
		ConvertMapToArray();
		return true;
	}
	return false;
}

void UUnrealInventoryComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UUnrealInventoryComponent, ReplicatedItemUnits);
}

void UUnrealInventoryComponent::ConvertMapToArray()
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

void UUnrealInventoryComponent::ConvertArrayToMap()
{
	ItemUnits.Empty();
	for (const auto& Elem : ReplicatedItemUnits)
	{
		UE_LOG(LogTemp, Warning, TEXT("OnRep_ItemUnits: %s, %d"), *Elem.ItemCode, Elem.Count);
		ItemUnits.Add(Elem.ItemCode, Elem.Count);
	}
}

void UUnrealInventoryComponent::OnRep_ItemUnits()
{
	UE_LOG(LogTemp, Warning, TEXT("OnRep_ItemUnits"));
	ConvertArrayToMap(); // 클라이언트에서 배열을 맵으로 변환
	Notify(); // 옵저버 패턴으로 변경 사항을 알림
}