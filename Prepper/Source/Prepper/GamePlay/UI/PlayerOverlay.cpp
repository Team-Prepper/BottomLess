// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerOverlay.h"

#include "Kismet/GameplayStatics.h"
#include "Prepper/GamePlay/CharacterController/StatusComponent.h"
#include "Prepper/__Legacy/HUD/PrepperHUD.h"

void UPlayerOverlay::Update(const UStatusComponent& NewData)
{
	const float CurHP = NewData.GetCurHealth();
	const float MaxHP = NewData.GetMaxHealth();
	
	HealthBar->SetPercent(CurHP / MaxHP);
	
	const FString Text = FString::Printf(TEXT("%d/%d"),
		FMath::CeilToInt(CurHP), FMath::CeilToInt(MaxHP));
	HealthText->SetText(FText::FromString(Text));
}

void UPlayerOverlay::DrawCrosshair(const FHUDPackage& Crosshair)
{
	if(!GEngine) return;
	
	FVector2D ViewportSize;
	GEngine->GameViewport->GetViewportSize(ViewportSize);
	const FVector2D ViewportCenter(ViewportSize.X/ 2.f, ViewportSize.Y / 2.f);
	float SpreadScaled = CrosshairSpreadMax * Crosshair.CrosshairSpread;
	
	DrawCrosshairUnit(Crosshair.CrosshairCenter, ViewportCenter, FVector2D(0.f, 0.f), Crosshair.CrosshairColor);
	DrawCrosshairUnit(Crosshair.CrosshairLeft, ViewportCenter, FVector2D(-SpreadScaled, 0.f), Crosshair.CrosshairColor);
	DrawCrosshairUnit(Crosshair.CrosshairRight, ViewportCenter, FVector2D(SpreadScaled, 0.f), Crosshair.CrosshairColor);
	DrawCrosshairUnit(Crosshair.CrosshairTop, ViewportCenter, FVector2D(0.f, -SpreadScaled), Crosshair.CrosshairColor);
	DrawCrosshairUnit(Crosshair.CrosshairBottom, ViewportCenter, FVector2D(0.f, SpreadScaled), Crosshair.CrosshairColor);
	
}

void UPlayerOverlay::DrawCrosshairUnit(UTexture2D* Texture, const FVector2D& ViewportCenter, const FVector2D& Spread, const FLinearColor& CrosshairColor)
{
	if (!Texture) return;
	
	const float TextureWidth = Texture->GetSizeX();
	const float TextureHeight =  Texture->GetSizeY();

	const APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	PC->GetHUD()->DrawTexture(
		Texture,
		ViewportCenter.X - (TextureWidth / 2.f) + Spread.X,
		ViewportCenter.Y - (TextureHeight / 2.f) + Spread.Y,
		TextureWidth,TextureWidth,
		0.f,0.f,
		1.f,1.f,
		CrosshairColor
		);
}