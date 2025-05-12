// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ItemMeta.generated.h"

class AInventoryInteractableItem;

USTRUCT(BlueprintType)
struct PREPPER_API FItemMeta
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ItemInfo")
	FText ItemName;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ItemInfo")
	TObjectPtr<UTexture2D> ItemIcon;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ItemInfo")
	TSubclassOf<AActor> ItemObject;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ItemInfo")
	TSubclassOf<AInventoryInteractableItem> ItemInteraction;
	
	FItemMeta();
	FItemMeta(const FText& Name, UTexture2D* Icon, TSubclassOf<AActor> Object, TSubclassOf<AInventoryInteractableItem> Interaction);
	
};
