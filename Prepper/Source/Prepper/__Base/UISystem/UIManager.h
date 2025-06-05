#pragma once

#include "GUIFullScreen.h"
#include "Blueprint/UserWidget.h"
#include "../USingleton.h"

class UIManager : public USingleton<UIManager>
{
private:
	TMap<FString, TSubclassOf<UUserWidget>> UIDataMap;
	TArray<IGUIFullScreen*> FullScreenStack;
	IGUIFullScreen* NowDisplay;
public:
	UIManager();
	
	IGUIFullScreen* GetNowDisplay() { return NowDisplay;}
	
	void EnrollmentGUI(IGUIFullScreen* NewFullScreen);
	void Pop();
	
	template <typename T>
	T* OpenGUI(APlayerController* Controller, const FString& GuiName);
	
};

// template을 사용할 경우 선언과 구현 파일을 나누면 LINK2019 문제가 발생하는 것으로 보임
// 참고: https://devhoma.tistory.com/49
template <typename T>
T* UIManager::OpenGUI(APlayerController* Controller, const FString& GuiName)
{
	
	if (UIDataMap.Num() == 0) return nullptr;
	if (!UIDataMap.Contains(GuiName)) return nullptr;

	TSubclassOf<UUserWidget> TargetUI = UIDataMap[GuiName];
	if (TargetUI == nullptr) return nullptr;
	
	T* Retval = CreateWidget<T>(Controller, TargetUI);
	Retval->AddToViewport();
	Retval->Open();
	
	return Cast<T>(Retval);
}
