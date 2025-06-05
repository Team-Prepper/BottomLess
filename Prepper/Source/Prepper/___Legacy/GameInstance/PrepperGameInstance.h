// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "PrepperGameInstance.generated.h"

/**
 * 
 */
UCLASS()
class PREPPER_API UPrepperGameInstance : public UGameInstance
{
	GENERATED_BODY()

private:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI", meta = (AllowPrivateAccess = "true"))
	TMap<FString, TSubclassOf<class AInventoryInteractableItem>> InteractableItems;

	UPROPERTY(EditAnywhere, Category="Item")
	TObjectPtr<UDataTable> ItemDataTable;
	UPROPERTY(EditAnywhere, Category="Item")
	TObjectPtr<UDataTable> ItemCombinationDataTable;
	UPROPERTY(EditAnywhere, Category="Item")
	TObjectPtr<UDataTable> WeaponDataTable;

public:
	UPrepperGameInstance();

	virtual void Init() override;
};
