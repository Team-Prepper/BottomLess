// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "EscapeActionComponent.generated.h"


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class PREPPER_API UEscapeActionComponent : public UActorComponent
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, Category = "Player HUD")
	TSubclassOf<UUserWidget> SettingWidgetClass;
	UPROPERTY()
	TObjectPtr<UUserWidget> SettingWidget;

public:
	// Sets default values for this component's properties
	UEscapeActionComponent();

	void EscapePressed();
	
};
