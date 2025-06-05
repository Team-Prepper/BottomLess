#pragma once
#include "ItemEffect.h"
#include "Prepper/GamePlay/PlayerAbility.h"

class DrinkEffect : public IItemEffect
{
	int Value;
public:
	DrinkEffect(int value)
	{
		Value = value;
	}
	virtual void Action(IPlayerAbility* Target) override
	{
		Target->Drink(Value);
	} 
};
