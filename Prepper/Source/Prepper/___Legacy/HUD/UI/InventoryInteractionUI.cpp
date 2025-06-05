#include "InventoryInteractionUI.h"

#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Prepper/GamePlay/Item/ItemManager.h"
#include "Prepper/GamePlay/Item/ItemUIData.h"

class UItemUIData;

void UInventoryInteractionUI::InitializeWidget(const FString& InitItemCode)
{
	UItemUIData* Data = NewObject<UItemUIData>(GetWorld(), UItemUIData::StaticClass());
	if(ItemManager::GetInstance()->GetItemData(InitItemCode, Data->TextureIcon, Data->ItemName))
	{
		ItemIcon->SetBrushFromTexture(Data->TextureIcon);
		ItemName->SetText(Data->ItemName);
		ItemCode = InitItemCode;
	}
	// interaction 키를 눌러 사용하세요 등의 멘트 필요 
}
