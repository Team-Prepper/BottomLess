#include "CharacterOverlay.h"

#include "Components/CanvasPanel.h"
#include "Components/Image.h"
#include "Components/VerticalBox.h"
#include "Engine/Canvas.h"
#include "Prepper/__Legacy/Item/ItemManager.h"

void UCharacterOverlay::Update(const GaugeValue<float>& NewData)
{
	HealthBar->SetPercent(NewData.GetRatio());
	
	const FString Text = FString::Printf(TEXT("%d/%d"),
		FMath::CeilToInt(NewData.GetCurValue()), FMath::CeilToInt(NewData.GetMaxValue()));
	HealthText->SetText(FText::FromString(Text));
	
}

void UCharacterOverlay::Update(const TArray<FItemConvertData>& NewData)
{
	ClearAllItemIcons();
	for (const auto& Elem : NewData)
	{
		UE_LOG(LogTemp, Log, TEXT("Key: %s, Value: %d"), *Elem.ItemCode, Elem.Count);
		AddItemIcon(ItemManager::GetInstance()->GetItemMeta(Elem.ItemCode)->ItemIcon);
	}
}

void UCharacterOverlay::AddItemIcon(UTexture2D* ItemIconTexture)
{
	if (ItemBox && ItemIconTexture)
	{
		// 새로운 UImage 위젯 생성
		UImage* NewItemIcon = NewObject<UImage>(this);

		// UImage에 텍스처 설정
		NewItemIcon->SetBrushFromTexture(ItemIconTexture);

		// Vertical Box에 추가
		ItemBox->AddChild(NewItemIcon);
	}
}

void UCharacterOverlay::ClearAllItemIcons()
{
	if (ItemBox)
	{
		ItemBox->ClearChildren();
	}
}
