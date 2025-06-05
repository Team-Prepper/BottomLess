// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#define MAX_QUICK_SLOT 5
#define DEFAULT_QUICK_SLOT_ITEM FString("")

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Prepper/__Base/ObserverPattern/Subject.h"
#include "InventoryComponent.generated.h"

class IPlayerAbility;

USTRUCT(BlueprintType)
struct FItemConvertData
{
	GENERATED_BODY()

	UPROPERTY()
	FString ItemCode;

	UPROPERTY()
	uint8 Count;
};

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class PREPPER_API UInventoryComponent : public UActorComponent, public ISubject<UInventoryComponent*>
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	uint8 InventorySize = 16;

	bool TryAddMapExist(TMap<FString, uint8>& Target, const FString& ItemCode, const int Count);
	bool TryUseMapExist(TMap<FString, uint8>& Target, const FString& ItemCode, const int Count);
	
protected:
	IPlayerAbility* Owner;
	FString QuickSlotItem[MAX_QUICK_SLOT];
	TMap<FString, uint8> ItemUnits;
	
public:
	// Sets default values for this component's properties
	UInventoryComponent();

	virtual void SetOwner(IPlayerAbility* NewOwner);
	virtual void ChangingInventory(TObjectPtr<UInventoryComponent> NewInventory);
	
	virtual bool TryAddItem(const FString& ItemCode, const int Count);
	virtual bool TryUseItem(const FString& ItemCode, const int Count);
	virtual bool TryDiscardItem(const FString& ItemCode, const int Count);
	virtual int TryGetItemCount(const FString& ItemCode);

	virtual void QuickSlotAdd(const FString& ItemCode, const int Idx);
	virtual void QuickSlotRemove(const int Idx);
	virtual void UseItemAtQuickSlot(const int Idx);

	virtual TArray<FItemConvertData> GetIter() const;
	virtual TArray<FItemConvertData> GetQuickSlotIter() const;

protected:
	TSet<IObserver<UInventoryComponent*>*> Observers;
public:
	
	virtual void Attach(IObserver<UInventoryComponent*>* Observer) override;
	virtual void Detach(IObserver<UInventoryComponent*>* Observer) override;
	virtual void Notify();
};
