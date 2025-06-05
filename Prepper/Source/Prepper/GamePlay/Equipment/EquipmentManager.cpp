#include "EquipmentManager.h"


#include "EquipmentData/EquipmentData.h"

EquipmentManager::EquipmentManager()
{
	/*
	TObjectPtr<UDataTable> WeaponDataTable;
	DataTableGetter::GetDataTable("WeaponDataTable", WeaponDataTable);

	Initial(WeaponDataTable);
	*/
}

EquipmentManager::~EquipmentManager()
{
}

void EquipmentManager::Initial(const TObjectPtr<UDataTable> EquipmentDataTable)
{
	if (!EquipmentDataTable) return;
	
	TArray<FEquipmentData*> Arr;
	EquipmentDataTable->GetAllRows(TEXT("GetAllRows"), Arr);
		
	for (int i = 0; i < Arr.Num(); ++i)
	{
		EquipmentData.Add(Arr[i]->EquipmentCode, Arr[i]);
	}
	UE_LOG(LogTemp, Warning, TEXT("EquipmentDataTableLoad: %d"), Arr.Num());
}

TObjectPtr<UTexture2D> EquipmentManager::GetEquipmentIcon(const FString& EquipmentCode)
{
	if (!EquipmentData.Contains(EquipmentCode)) return nullptr;
	return (*EquipmentData.Find(EquipmentCode))->EquipmentIcon;
}

