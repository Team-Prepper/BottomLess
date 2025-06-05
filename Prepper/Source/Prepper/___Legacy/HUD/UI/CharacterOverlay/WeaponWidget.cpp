// Fill out your copyright notice in the Description page of Project Settings.


#include "WeaponWidget.h"

#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Prepper/GamePlay/Character/Component/BCombatComponent.h"
#include "Prepper/GamePlay/Equipment/EquipmentManager.h"

void UWeaponWidget::Update(const GaugeValue<int>& NewData)
{
	const FString AmmoText = FString::Printf(TEXT("%d"), NewData.GetCurValue());
	WeaponAmmoValue->SetText(FText::FromString(AmmoText));
	
	const FString CarriedText = FString::Printf(TEXT("%d"), NewData.GetMaxValue());
	CarriedAmmoValue->SetText(FText::FromString(CarriedText));
}

void UWeaponWidget::Update(const FString& NewData)
{
	UE_LOG(LogTemp, Warning, TEXT("Draw %s"), *NewData);
	WeaponIcon->SetBrushFromTexture(EquipmentManager::GetInstance()->GetEquipmentIcon(NewData));
}

void UWeaponWidget::Update(const UBCombatComponent& NewData)
{
	WeaponIcon->SetBrushFromTexture(EquipmentManager::GetInstance()
		->GetEquipmentIcon(NewData.GetEquippedWeaponCode()));
	
	const FString AmmoText = NewData.GetAmmoValue();
	WeaponAmmoValue->SetText(FText::FromString(AmmoText));
	
}