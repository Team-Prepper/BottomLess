// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Prepper/__Legacy/HUD/UI/CharacterOverlay/CharacterOverlay.h"
#include "PlayerOverlay.generated.h"

struct FHUDPackage;
/**
 * 
 */
UCLASS()
class PREPPER_API UPlayerOverlay : public UUserWidget
{
	GENERATED_BODY()
private:
	UPROPERTY(EditAnywhere, meta = (BindWidget), Category=Health)
	UProgressBar* HealthBar;
	UPROPERTY(EditAnywhere, meta = (BindWidget), Category=Health)
	UTextBlock* HealthText;
	UPROPERTY(EditAnywhere)
	float CrosshairSpreadMax = 16.f;
	
public:
	void SetHP(float CurHP, float MaxHP);
	void DrawCrosshair(const FHUDPackage& Crosshair);
	void DrawCrosshairUnit(UTexture2D* Texture, const FVector2D& ViewportCenter, const FVector2D& Spread,
	                              const FLinearColor& CrosshairColor);
};
