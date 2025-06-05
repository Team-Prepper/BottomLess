// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "Prepper/___Legacy/Character/PlayerCharacter.h"
#include "UI/CharacterOverlay/CharacterOverlay.h"
#include "PrepperHUD.generated.h"

USTRUCT(BlueprintType)
struct FHUDPackage
{
	GENERATED_BODY()

	UPROPERTY()
	TObjectPtr<UTexture2D> CrosshairCenter;
	UPROPERTY()
	TObjectPtr<UTexture2D> CrosshairLeft;
	UPROPERTY()
	TObjectPtr<UTexture2D> CrosshairRight;
	UPROPERTY()
	TObjectPtr<UTexture2D> CrosshairTop;
	UPROPERTY()
	TObjectPtr<UTexture2D> CrosshairBottom;
	
	float CrosshairSpread;

	FLinearColor CrosshairColor;
	
};

UCLASS()
class PREPPER_API APrepperHUD : public AHUD
{
	GENERATED_BODY()
	
public:
	virtual void DrawHUD() override;

	void AddCharacterOverlay();
	UPROPERTY()
	class UAnnouncement* Announcement;

	void AddAnnouncement();

	UFUNCTION()
	void ResetCrossHair();

protected:
	virtual void BeginPlay() override;
	
private:

	UPROPERTY(EditAnywhere, Category = "Player HUD")
	TSubclassOf<UUserWidget> InventoryHUDClass;
	
	UPROPERTY(EditAnywhere, Category = "Annoucement")
	TSubclassOf<UUserWidget> AnnouncementClass;
	
	FHUDPackage HUDPackage;
	bool IsInventoryVisible = false;
	
	UPROPERTY(EditAnywhere)
	float CrosshairSpreadMax = 16.f;

	void DrawCrosshair();
	void DrawCrosshairUnit(UTexture2D* Texture, const FVector2D& ViewportCenter, const FVector2D& Spread, const FLinearColor& CrosshairColor);

public:
	FORCEINLINE void SetHUDPackage(const FHUDPackage& Package) { HUDPackage = Package; }
	
};
