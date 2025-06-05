// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "SurvivorGameOverUI.generated.h"

class UButton;
/**
 * 
 */
UCLASS()
class PREPPER_API USurvivorGameOverUI : public UUserWidget
{
	GENERATED_BODY()
	
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UButton> RetryButton;
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UButton> CloseButton;

public:
	UFUNCTION()
	void Retry();

	UFUNCTION(Server, Reliable)
	void ServerRespawnRequest(ACharacter* ElimmedCharacter, AController* ElimmedController);
	
	virtual void NativeOnInitialized() override;
};
