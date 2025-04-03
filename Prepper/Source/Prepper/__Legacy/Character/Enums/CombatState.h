#pragma once

UENUM(BlueprintType)
enum class ECombatState : uint8
{
	ECS_Unoccupied UMETA(DisplayName = "Unoccupied"),
	ECS_Fire UMETA(DisplayName = "Fire"),
	ECS_Reloading UMETA(DisplayName = "Reloading"),
	ECS_SwappingWeapons UMETA(DisplayName = "SwappingWeapons"),

	ECS_MAX UMETA(DisplayName = "DefaultMAX")
};