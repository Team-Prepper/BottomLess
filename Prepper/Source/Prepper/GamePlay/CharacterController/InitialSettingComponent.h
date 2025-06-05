// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "InitialSettingComponent.generated.h"


class UWeaponWidget;
class UPlayerOverlay;
class AUnrealPlayerController;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class PREPPER_API UInitialSettingComponent : public UActorComponent
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, Category = "Player HUD")
	TSubclassOf<UPlayerOverlay>  PlayerOverlayClass;
	UPROPERTY()
	TObjectPtr<UPlayerOverlay> PlayerOverlay;
	
	UPROPERTY(EditAnywhere, Category="Player HUD")
	TSubclassOf<UWeaponWidget> WeaponOverlayClass;
	UPROPERTY()
	TObjectPtr<UWeaponWidget> WeaponOverlay;
	
public:
	// Sets default values for this component's properties
	UInitialSettingComponent();
	
protected:
	TObjectPtr<AUnrealPlayerController> TargetCC;
	
	virtual void Attach();
	virtual void Detach();
public:
	virtual void WidgetSetting(TObjectPtr<AUnrealPlayerController> Target);
	virtual void Initial(TObjectPtr<AUnrealPlayerController> Target);
};
