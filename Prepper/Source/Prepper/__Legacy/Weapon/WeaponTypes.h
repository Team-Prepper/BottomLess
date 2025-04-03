#pragma once

#define TRACE_LEN 80000.f

UENUM(BlueprintType)
enum class EWeaponType : uint8
{
	EWT_AssaultRifle UMETA(DisplayName = "Assault Rifle"),
	EWT_RocketLauncher UMETA(DisplayName = "Rocket Launcher"),
	EWT_Revolver UMETA(DisplayName = "Revolver"),
	EWT_SMG UMETA(DisplayName = "SMG"),
	EWT_Shotgun UMETA(DisplayName = "Shotgun"),
	EWT_SniperRifle UMETA(DisplayName = "Sniper Rifle"),
	EWT_MiniGun UMETA(DisplayName = "MiniGun"),
	EWT_MeleeWeaponBlunt UMETA(DisplayName = "MeleeWeaponBlunt"),
	EWT_MeleeWeaponSword UMETA(DisplayName = "MeleeWeaponSword"),
	
	EWT_MAX UMETA(DisplayName = "DefaultMAX")
};

UENUM(BlueprintType)
enum class EWeaponState : uint8
{
	EWS_Initial UMETA(DisplayName = "Initial State"),
	EWS_Equipped UMETA(DisplayName = "Equipped"),
	EWS_Holstered UMETA(DisplayName = "Holstered"),
	EWS_Dropped UMETA(DisplayName = "Dropped"),
	
	EWS_MAX UMETA(DisplayName = "Default MAX")
};
