// Fill out your copyright notice in the Description page of Project Settings.


#include "GUIFullScreenWidget.h"

#include "UIManager.h"

void UGUIFullScreenWidget::SetOn()
{
	SetVisibility(ESlateVisibility::Visible);
}

void UGUIFullScreenWidget::SetOff()
{
	SetVisibility(ESlateVisibility::Hidden);
}

void UGUIFullScreenWidget::Open()
{
	SetOn();
	UIManager::GetInstance()->EnrollmentGUI(this);
}

void UGUIFullScreenWidget::Close()
{
	UIManager::GetInstance()->Pop();

	while (PopUpStack.Num() > 0)
	{
		NowPopUp->Close();
	}

}

void UGUIFullScreenWidget::AddPopUp(IGUIPopUp* PopUp)
{
	if (NowPopUp != nullptr)
	{
		PopUpStack.Add(NowPopUp);
		NowPopUp->SetOff();
	}

	NowPopUp = PopUp;
}

void UGUIFullScreenWidget::PopPopUp()
{
	if (PopUpStack.Num() == 0) {
		NowPopUp = nullptr;
		return;
	}

	NowPopUp = PopUpStack[PopUpStack.Num() - 1];
	NowPopUp->SetOn();
	PopUpStack.RemoveAt(PopUpStack.Num() - 1);
	
}
