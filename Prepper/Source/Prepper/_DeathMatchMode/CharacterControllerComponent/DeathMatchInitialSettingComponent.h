// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Prepper/GamePlay/CharacterController/InitialSettingComponent.h"
#include "Prepper/__Base/ObserverPattern/Observer.h"
#include "DeathMatchInitialSettingComponent.generated.h"

class ADeathMatchPlayerState;
class UAnnouncement;
class UDeathMatchWidget;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class PREPPER_API UDeathMatchInitialSettingComponent : public UInitialSettingComponent, public IObserver<ADeathMatchPlayerState>
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category = "Player HUD")
	TSubclassOf<UDeathMatchWidget> DeathMatchWidgetClass;
	UPROPERTY()
	TObjectPtr<UDeathMatchWidget> DeathMatchWidget;
	
	UPROPERTY(EditAnywhere, Category = "Player HUD")
	TSubclassOf<UAnnouncement> AnnounceWidgetClass;
	UPROPERTY()
	TObjectPtr<UAnnouncement> AnnounceWidget;
	
public:
	// Sets default values for this component's properties
	UDeathMatchInitialSettingComponent();
	virtual void Initial(TObjectPtr<AUnrealPlayerController> Target) override;

	virtual void Update(const ADeathMatchPlayerState& NewData) override;
};
