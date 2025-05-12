// Fill out your copyright notice in the Description page of Project Settings.


#include "ItemMeta.h"

FItemMeta::FItemMeta()
{
	FItemMeta(FText::FromString(""), nullptr, nullptr, nullptr);
}

FItemMeta::FItemMeta(const FText& Name, UTexture2D* Icon, TSubclassOf<AActor> Object,
                     TSubclassOf<AInventoryInteractableItem> Interaction)
{
}
