// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Item.h"

#include "Effect/DrinkEffect.h"
#include "Effect/EatEffect.h"
#include "Effect/HealEffect.h"

IItemEffect* FItem::StringToItemEffect(const FString& Value)
{
		
	TArray<FString> CodePiece; 
	const TCHAR* Delims[] = { TEXT(":") };
	const int32 ArraySize = Value.ParseIntoArray(CodePiece, Delims, 1);
	
	if (CodePiece[0].Compare("Heal") == 0)
		return new HealEffect(FCString::Atoi(*CodePiece[1]));
	if (CodePiece[0].Compare("Eat") == 0)
		return new EatEffect(FCString::Atoi(*CodePiece[1]));
	if (CodePiece[0].Compare("Drink") == 0)
		return new DrinkEffect(FCString::Atoi(*CodePiece[1]));

	return new HealEffect(FCString::Atoi(*CodePiece[1]));
		
}

FItem::FItem()
{
	FItem("");
		
}

FItem::FItem(const FString& ItemEffectCode)
{
	TArray<FString> CodePiece; 
	const TCHAR* Delims[] = { TEXT(",") };
	const int32 ArraySize = ItemEffectCode.ParseIntoArray(CodePiece, Delims, 1);

	for (int i = 0; i < ArraySize; i++)
	{
		ItemEffects.Add(StringToItemEffect(CodePiece[i]));
	}
}

void FItem::Use(IPlayerAbility* Target)
{
	UE_LOG(LogTemp, Warning, TEXT("Use"));
	for (IItemEffect* Effect : ItemEffects)
	{
		Effect->Action(Target);
	}
}