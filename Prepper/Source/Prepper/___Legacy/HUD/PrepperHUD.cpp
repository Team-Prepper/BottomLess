#include "PrepperHUD.h"

#include "UI/Announcement.h"
#include "Blueprint/UserWidget.h"
#include "GameFramework/PlayerController.h"

void APrepperHUD::BeginPlay()
{
	Super::BeginPlay();
}

void APrepperHUD::AddCharacterOverlay()
{
	APlayerController* PlayerController = GetOwningPlayerController();
	
	if(!PlayerController) return;
}

void APrepperHUD::AddAnnouncement()
{
	APlayerController* PlayerController = GetOwningPlayerController();
	if(PlayerController && AnnouncementClass)
	{
		Announcement = CreateWidget<UAnnouncement>(PlayerController, AnnouncementClass);
		Announcement->AddToViewport();
	}
}

void APrepperHUD::DrawHUD()
{
	Super::DrawHUD();
	DrawCrosshair();
}

void APrepperHUD::DrawCrosshair()
{
	FVector2D ViewportSize;
	if(!GEngine) return;
	
	GEngine->GameViewport->GetViewportSize(ViewportSize);
	
	const FVector2D ViewportCenter(ViewportSize.X/ 2.f, ViewportSize.Y / 2.f);
	float SpreadScaled = CrosshairSpreadMax * HUDPackage.CrosshairSpread;

	DrawCrosshairUnit(HUDPackage.CrosshairCenter, ViewportCenter, FVector2D(0.f, 0.f), HUDPackage.CrosshairColor);
	DrawCrosshairUnit(HUDPackage.CrosshairLeft, ViewportCenter, FVector2D(-SpreadScaled, 0.f), HUDPackage.CrosshairColor);
	DrawCrosshairUnit(HUDPackage.CrosshairRight, ViewportCenter, FVector2D(SpreadScaled, 0.f), HUDPackage.CrosshairColor);
	DrawCrosshairUnit(HUDPackage.CrosshairTop, ViewportCenter, FVector2D(0.f, -SpreadScaled), HUDPackage.CrosshairColor);
	DrawCrosshairUnit(HUDPackage.CrosshairBottom, ViewportCenter, FVector2D(0.f, SpreadScaled), HUDPackage.CrosshairColor);
	
}

void APrepperHUD::DrawCrosshairUnit(UTexture2D* Texture, const FVector2D& ViewportCenter, const FVector2D& Spread, const FLinearColor& CrosshairColor)
{
	if (!Texture) return;
	
	const float TextureWidth = Texture->GetSizeX();
	const float TextureHeight =  Texture->GetSizeY();

	DrawTexture(
		Texture,
		ViewportCenter.X - (TextureWidth / 2.f) + Spread.X,
		ViewportCenter.Y - (TextureHeight / 2.f) + Spread.Y,
		TextureWidth,TextureWidth,
		0.f,0.f,
		1.f,1.f,
		CrosshairColor
		);
}

void APrepperHUD::ResetCrossHair()
{
	HUDPackage.CrosshairCenter = nullptr;
	HUDPackage.CrosshairLeft = nullptr;
	HUDPackage.CrosshairRight = nullptr;
	HUDPackage.CrosshairTop = nullptr;
	HUDPackage.CrosshairBottom = nullptr;
}