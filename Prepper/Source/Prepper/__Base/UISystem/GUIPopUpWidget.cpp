// Fill out your copyright notice in the Description page of Project Settings.


#include "GUIPopUpWidget.h"

#include "UIManager.h"

void UGUIPopUpWidget::SetOn()
{
	SetVisibility(ESlateVisibility::Visible);
}

void UGUIPopUpWidget::SetOff()
{
	SetVisibility(ESlateVisibility::Hidden);
}

void UGUIPopUpWidget::Open()
{
	SetOn();
	IGUIFullScreen* Parent = UIManager::GetInstance()->GetNowDisplay();

	if (Parent == nullptr)
	{
		Close();
		return;
	}

	Parent->AddPopUp(this);
}

void UGUIPopUpWidget::Close()
{
	IGUIFullScreen* Parent = UIManager::GetInstance()->GetNowDisplay();
	
	if (Parent == nullptr) return;
	Parent->PopPopUp();
}
