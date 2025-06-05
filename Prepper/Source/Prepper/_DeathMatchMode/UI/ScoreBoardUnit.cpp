#include "ScoreBoardUnit.h"

#include "ScoreBoardUnitData.h"
#include "Components/TextBlock.h"

void UScoreBoardUnit::NativeOnInitialized()
{
	Super::NativeOnInitialized();
}

void UScoreBoardUnit::NativeOnListItemObjectSet(UObject* ListItemObject)
{
	IUserObjectListEntry::NativeOnListItemObjectSet(ListItemObject);
	
	const UScoreBoardUnitData* Data = Cast<UScoreBoardUnitData>(ListItemObject);

	PlayerName->SetText(FText::FromString(Data->PlayerName));
	Score->SetText(FText::FromString(FString::FromInt(Data->Score)));
	Defeat->SetText(FText::FromString(FString::FromInt(Data->Defeat)));
}
