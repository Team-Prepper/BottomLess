// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "SurvivorSettingUI.generated.h"

class UButton;
/**
 * 
 */
UCLASS()
class PREPPER_API USurvivorSettingUI : public UUserWidget
{
	GENERATED_BODY()

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> SaveButton;

public:
	void NativeOnInitialized();

	UFUNCTION()
	void DataSave();
	
	
};
