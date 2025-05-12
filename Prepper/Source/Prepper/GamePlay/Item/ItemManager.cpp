#include "ItemManager.h"
// Fill out your copyright notice in the Description page of Project Settings.

#include "UObject/ConstructorHelpers.h"
#include "ItemData/ItemData.h"
#include "Prepper/_Base/DataTableGetter.h"

FString ItemManager::ItemCombineCode(const FString& Code1, const FString& Code2)
{
	if (Code1.Compare(Code2) > 0)
	{
		return Code1 + Code2;
	}

	return Code2 + Code1;
}

ItemManager::ItemManager()
{
	TObjectPtr<UDataTable> ItemDataTable;
	DataTableGetter::GetDataTable("ItemDataTable", ItemDataTable);
	
	TObjectPtr<UDataTable> ItemCombinationDataTable;
	DataTableGetter::GetDataTable("ItemCombinationDataTable", ItemCombinationDataTable);

	Initial(ItemDataTable, ItemCombinationDataTable);
	
}

ItemManager::~ItemManager()
{
}

void ItemManager::Initial(const TObjectPtr<UDataTable> ItemDataTable, const TObjectPtr<UDataTable> ItemCombinationDataTable)
{
	
	if (ItemDataTable)
	{
		TArray<FItemData*> arr;
		ItemDataTable->GetAllRows(TEXT("GetAllRows"), arr);
		
		for (int i = 0; i < arr.Num(); ++i)
		{
			ItemData.Add(arr[i]->ItemCode, arr[i]->GetItem());
			ItemMetaData.Add(arr[i]->ItemCode, arr[i]->GetItemMeta());
		}
		UE_LOG(LogTemp, Warning, TEXT("ItemDataTableLoad: %d"), arr.Num());
		
	}
	
	if (ItemCombinationDataTable)
	{
		TArray<FItemCombinationData*> arr;
		ItemCombinationDataTable->GetAllRows(TEXT("GetAllRows"), arr);
		
		for (int i = 0; i < arr.Num(); ++i)
		{
			CombinationData.Add(arr[i]->OutputItemCode, *arr[i]);
		}
	}
}

bool ItemManager::GetItemData(const FString& ItemCode, UTexture2D*& ItemIcon, FText& ItemName)
{
	if (!ItemData.Contains(ItemCode)) return false;

	const FItemMeta* Data = ItemMetaData.Find(ItemCode);

	ItemIcon = Data->ItemIcon;
	ItemName = Data->ItemName;
	
	return true;
}

FItem* ItemManager::GetItem(const FString& ItemCode)
{
	if (!ItemData.Contains(ItemCode)) return nullptr;

	return ItemData.Find(ItemCode);
}

FItemMeta* ItemManager::GetItemMeta(const FString& ItemCode)
{
	if (!ItemMetaData.Contains(ItemCode)) return nullptr;

	return ItemMetaData.Find(ItemCode);
	
}

TObjectPtr<AActor> ItemManager::SpawnItem(UWorld* World, const FString& ItemCode)
{
	if (World == nullptr) return nullptr;
	return World->SpawnActor<AActor>(ItemMetaData.Find(ItemCode)->ItemObject);
}

TObjectPtr<AInventoryInteractableItem> ItemManager::SpawnItemInteraction(UWorld* World, const FString& ItemCode)
{
	if (World == nullptr) return nullptr;
	return World->SpawnActor<AInventoryInteractableItem>(ItemMetaData.Find(ItemCode)->ItemInteraction);
}

TArray<FString> ItemManager::GetPossibleCombination() const
{
	TArray<FString> Retval;
	CombinationData.GetKeys(Retval);
	
	return Retval;
}

bool ItemManager::TryCombinationItem(const FString& ResultCode, FString& ItemCode1, int& Item1Cnt, FString& ItemCode2,
                                     int& Item2Cnt)
{
	if (!CombinationData.Contains(ResultCode))
	{
		return false;
	}
	FItemCombinationData Data = CombinationData[ResultCode];
	
	ItemCode1 = Data.InputItemCode1;
	Item1Cnt = Data.Item1NeedCnt;
	ItemCode2 = Data.InputItemCode2;
	Item2Cnt = Data.Item2NeedCnt;
	
	return true;
	
}
/*
bool ItemManager::TryCombinationItem(const FString& ItemCode1, const FString& ItemCode2, FString& ResultCode)
{
	const FString CombinationCode = ItemCombineCode(ItemCode1, ItemCode2);
	
	if (!CombinationData.Contains(CombinationCode))
	{
		return false;
	}

	ResultCode = CombinationData.Find(CombinationCode)->OutputItemCode;

	return true;
}*/

int ItemManager::CountCombinationData()
{
	return CombinationData.Num();
}
