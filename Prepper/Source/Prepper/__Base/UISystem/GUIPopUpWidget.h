// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GUI.h"
#include "GUIPopUp.h"
#include "Blueprint/UserWidget.h"
#include "GUIPopUpWidget.generated.h"

/**
 * 
 */
UCLASS()
class PREPPER_API UGUIPopUpWidget : public UUserWidget, public IGUIPopUp
{
	GENERATED_BODY()
public:
	virtual void SetOn() override;
	virtual void SetOff() override;

	virtual void Open() override;
	virtual void Close() override;
};
