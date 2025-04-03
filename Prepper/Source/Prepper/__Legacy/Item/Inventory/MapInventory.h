#pragma once

#define MAX_QUICK_SLOT 5

#include "CoreMinimal.h"
#include "Inventory.h"
#include "Components/ActorComponent.h"
#include "Prepper/_Base/ObserverPattern/Subject.h"
#include "Net/UnrealNetwork.h" // 네트워크 관련 헤더 추가
#include "MapInventory.generated.h"

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class PREPPER_API UMapInventory : public UActorComponent, public IInventory, public ISubject<IInventory*>
{
	GENERATED_BODY()
private:
	IPlayerAbility* Owner;

	uint8 BulletCount;

	// 복제 가능한 TArray로 ItemUnits를 대체
	UPROPERTY(ReplicatedUsing = OnRep_ItemUnits)
	TArray<FItemConvertData> ReplicatedItemUnits;

	TMap<FString, uint8> ItemUnits; // 실제 아이템 저장용

	FString QuickSlotItem[MAX_QUICK_SLOT];

	UPROPERTY(EditAnywhere)
	uint8 InventorySize = 16;

	TSet<IObserver<IInventory*>*> Observers;

	bool TryAddMapExist(TMap<FString, uint8>& Target, const FString& ItemCode, const int Count);
	bool TryUseMapExist(TMap<FString, uint8>& Target, const FString& ItemCode, const int Count);

public:
	void SetOwner(IPlayerAbility* NewOwner);
	virtual void Attach(IObserver<IInventory*>* Observer) override;
	virtual void Detach(IObserver<IInventory*>* Observer) override;
	virtual void Notify();
	void ChangingInventory(TObjectPtr<UMapInventory> NewInventory);
	
	UMapInventory();
	virtual bool TryAddItem(const FString& ItemCode, const int Count) override;
	virtual bool TryUseItem(const FString& ItemCode, const int Count) override;
	virtual bool TryDiscardItem(const FString& ItemCode, const int Count) override;
	virtual int TryGetItemCount(const FString& ItemCode) override;

	virtual void QuickSlotAdd(const FString& ItemCode, const int Idx) override;
	virtual void QuickSlotRemove(const int Idx) override;
	virtual void UseItemAtQuickSlot(const int Idx) override;

	virtual TArray<FItemConvertData> GetIter() const override;
	virtual TArray<FItemConvertData> GetQuickSlotIter() const override;

	// 클라이언트에서 호출되는 RepNotify 함수
	UFUNCTION()
	void OnRep_ItemUnits();

	// 네트워크 복제를 위한 함수
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

private:
	// TMap을 TArray로 변환하는 함수
	void ConvertMapToArray();

	// TArray를 TMap으로 변환하는 함수
	void ConvertArrayToMap();
};
