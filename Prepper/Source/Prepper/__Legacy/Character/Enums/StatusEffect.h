#pragma once

#include "CoreMinimal.h"

UENUM(BlueprintType)
enum class EStatusEffect : uint8
{
	ESE_NONE	 = 0,										// 0000 0000
	ESE_HUNGRY   = 1 << 0 UMETA(DisplayName = "Hungry"),    // 0000 0001
	ESE_THIRSTY  = 1 << 1 UMETA(DisplayName = "Thirsty"),   // 0000 0010
	ESE_SICK     = 1 << 2 UMETA(DisplayName = "Sick"),	    // 0000 0100
	ESE_BLEEDING = 1 << 3 UMETA(DisplayName = "Bleeding"),  // 0000 1000
	ESE_FRACTURE = 1 << 4 UMETA(DisplayName = "Fracture"),  // 0001 0000
	ESE_INFECTED = 1 << 5 UMETA(DisplayName = "Infected"),  // 0010 0000

	ECS_MAX = 0xFF UMETA(DisplayName = "DefaultMAX")
};

class PREPPER_API FStatusEffect
{
public:
	FStatusEffect() : Flags(static_cast<uint8>(EStatusEffect::ESE_NONE)) {}

	bool HasEffect(EStatusEffect Effect) const { return (Flags & static_cast<uint8>(Effect)) != 0; }
	void AddEffect(EStatusEffect Effect) { Flags |= static_cast<uint8>(Effect); }
	void RemoveEffect(EStatusEffect Effect) { Flags &= ~static_cast<uint8>(Effect); }

	bool HasAnyEffect() const { return Flags != static_cast<uint8>(EStatusEffect::ESE_NONE); }
	void ClearAllEffects() { Flags = static_cast<uint8>(EStatusEffect::ESE_NONE); }

private:
	uint8 Flags;
};