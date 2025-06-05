// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Prepper/___Legacy/HUD/UI/CharacterOverlay/CharacterOverlay.h"
#include "PlayerOverlay.generated.h"

class APrepperHUD;
class UStatusComponent;
class IStatus;
struct FHUDPackage;
/**
 * 
 */
UCLASS()
class PREPPER_API UPlayerOverlay : public UUserWidget,
									public IObserver<UStatusComponent>
{
	GENERATED_BODY()
private:
	UPROPERTY(EditAnywhere, meta = (BindWidget), Category=Health)
	TObjectPtr<UProgressBar> HealthBar;
	UPROPERTY(EditAnywhere, meta = (BindWidget), Category=Health)
	TObjectPtr<UTextBlock> HealthText;
	UPROPERTY(EditAnywhere)
	float CrosshairSpreadMax = 16.f;

	UPROPERTY()
	TObjectPtr<APrepperHUD> TargetHUD;
	
public:
	virtual void Update(const UStatusComponent& NewData) override;
	void SetHUD(TObjectPtr<APrepperHUD> HUD);
	void DrawCrosshair(const FHUDPackage& Crosshair);
	void DrawCrosshairUnit(UTexture2D* Texture, const FVector2D& ViewportCenter, const FVector2D& Spread,
	                              const FLinearColor& CrosshairColor);
};
