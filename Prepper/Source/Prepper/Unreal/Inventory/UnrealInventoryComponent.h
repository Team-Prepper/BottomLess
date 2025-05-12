#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Prepper/GamePlay/Item/Inventory/InventoryComponent.h"
#include "UnrealInventoryComponent.generated.h"

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class PREPPER_API UUnrealInventoryComponent : public UInventoryComponent
{
	GENERATED_BODY()
	
	// 복제 가능한 TArray로 ItemUnits를 대체
	UPROPERTY(ReplicatedUsing = OnRep_ItemUnits)
	TArray<FItemConvertData> ReplicatedItemUnits;

	TSet<IObserver<UInventoryComponent*>*> Observers;

public:
	virtual void ChangingInventory(TObjectPtr<UInventoryComponent> NewInventory) override;
	
	UUnrealInventoryComponent();
	virtual bool TryAddItem(const FString& ItemCode, const int Count) override;
	virtual bool TryUseItem(const FString& ItemCode, const int Count) override;
	virtual bool TryDiscardItem(const FString& ItemCode, const int Count) override;

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
