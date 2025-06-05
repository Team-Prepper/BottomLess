// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GUIFullScreen.h"
#include "Blueprint/UserWidget.h"
#include "UObject/NoExportTypes.h"
#include "GUIFullScreenWidget.generated.h"

/**
 * 
 */
UCLASS()
class PREPPER_API UGUIFullScreenWidget : public UUserWidget, public IGUIFullScreen
{
	GENERATED_BODY()
	IGUIPopUp* NowPopUp;
	TArray<IGUIPopUp*> PopUpStack;
public:
	virtual void SetOn() override;
	virtual void SetOff() override;

	virtual void Open() override;

	UFUNCTION()
	virtual void Close() override;
	
	virtual void AddPopUp(IGUIPopUp* PopUp) override;
	virtual void PopPopUp() override;
	
};
