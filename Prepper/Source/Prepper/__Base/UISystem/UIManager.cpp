#include "UIManager.h"

#include "EnhancedInputSubsystemInterface.h"
#include "UIData.h"
#include "../DataTableGetter.h"
#include "UObject/ConstructorHelpers.h"

UIManager::UIManager()
{
	
	UE_LOG(LogTemp, Warning, TEXT("UIManager OnCreate"));
	
	NowDisplay = nullptr;

	TObjectPtr<UDataTable> UIDataTable;
	
	if (!DataTableGetter::GetDataTable("UIDataTable", UIDataTable)) return;
	
	TArray<FUIData*> UIData;
	UIDataTable->GetAllRows(TEXT("GetAllRows"), UIData);
		
	for (int i = 0; i < UIData.Num(); ++i)
	{
		UIDataMap.Add(UIData[i]->UIName, UIData[i]->TargetClass);
	}
}

void UIManager::EnrollmentGUI(IGUIFullScreen* NewFullScreen)
{
	if (NowDisplay == nullptr)
	{
		NowDisplay = NewFullScreen;
		return;

	}
	
	NowDisplay->SetOff();
	FullScreenStack.Add(NowDisplay);
	FullScreenStack.Add(NewFullScreen);

	Pop();
}

void UIManager::Pop()
{
	if (FullScreenStack.Num() < 1)
		return;

	NowDisplay = FullScreenStack[FullScreenStack.Num() - 1];
	FullScreenStack.RemoveAt(FullScreenStack.Num() - 1);
	NowDisplay->SetOn();

}
