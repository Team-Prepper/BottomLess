#pragma once

#include "Equipment.h"
#include "EquipmentData/EquipmentData.h"
#include "Prepper/_Base/Singleton.h"

class AEquipment;

class EquipmentManager : public Singleton<EquipmentManager>
{
public:
	TMap<FString, FEquipmentData*> EquipmentData;
public:
	EquipmentManager();
	virtual ~EquipmentManager() override;
	void Initial(const TObjectPtr<UDataTable> EquipmentDataTable);
	TObjectPtr<UTexture2D> GetEquipmentIcon(const FString& EquipmentCode);
	template <typename T>
	TObjectPtr<T> SpawnEquipment(UWorld* World, const FString& WeaponCode);
};

template <typename T>
TObjectPtr<T> EquipmentManager::SpawnEquipment(UWorld* World, const FString& EquipmentCode)
{
	if (World == nullptr) return nullptr;
	if (!EquipmentData.Contains(EquipmentCode)) return nullptr;
	return World->SpawnActor<T>((*EquipmentData.Find(EquipmentCode))->EquipmentClass);
}