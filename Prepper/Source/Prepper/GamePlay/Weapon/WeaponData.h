#pragma once

class UPlayerAimingEffect;

struct FWeaponData
{
private:
	FString WeaponCode;
	FName ReloadActionName = FName("AssaultRifle");

public:
	virtual ~FWeaponData() = default;
	virtual FString GetCode() { return WeaponCode; }
};
