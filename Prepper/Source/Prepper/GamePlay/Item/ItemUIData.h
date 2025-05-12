
// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ItemUIData.generated.h"

class UInventoryComponent;
UCLASS()
class PREPPER_API UItemUIData : public UObject
{
	GENERATED_BODY()
	
public:
	UItemUIData()
	{
		TargetInventory = nullptr;
		
		TextureIcon = nullptr;
		ItemCode = "";
		ItemName = FText::FromString("");
		ItemCount = 0;
		
	}
	UItemUIData(UTexture2D* Icon, const FString& Code, const FText& Name, int Count, UInventoryComponent* InventoryUI)
	{
		TargetInventory = InventoryUI;
		
		TextureIcon = Icon;
		ItemCode = Code;
		ItemName = Name;
		ItemCount = Count;
	}
	
	UInventoryComponent* TargetInventory;

	UPROPERTY()
	UTexture2D* TextureIcon;

	FString ItemCode;
	FText ItemName;
	uint8 ItemCount;
};
