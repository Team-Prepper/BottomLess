#pragma once

#include "Prepper/__Legacy/Item/Item.h"
#include "ItemData/ItemCombinationData.h"
#include "ItemData/ItemMeta.h"
#include "Prepper/_Base/Singleton.h"

class ItemManager : public Singleton<ItemManager>
{
private:
	TMap<FString, FItem> ItemData;
	TMap<FString, FItemMeta> ItemMetaData;
	TMap<FString, FItemCombinationData> CombinationData;
	
	FString ItemCombineCode(const FString& Code1, const FString& Code2);
public:
	ItemManager();
	virtual ~ItemManager() override;
	void Initial(const TObjectPtr<UDataTable> ItemDataTable, const TObjectPtr<UDataTable> ItemCombinationDataTable);
	
	bool GetItemData(const FString& ItemCode, UTexture2D*& ItemIcon, FText& ItemName);
	FItem* GetItem(const FString& ItemCode);
	FItemMeta* GetItemMeta(const FString& ItemCode);
	
	TObjectPtr<AActor> SpawnItem(UWorld* World, const FString& ItemCode);
	TObjectPtr<AInventoryInteractableItem> SpawnItemInteraction(UWorld* World, const FString& ItemCode);

	TArray<FString> GetPossibleCombination() const;
	bool TryCombinationItem(const FString& ResultCode, FString& ItemCode1, int& Item1Cnt, FString& ItemCode2, int& Item2Cnt);
	
	int CountCombinationData();
};
