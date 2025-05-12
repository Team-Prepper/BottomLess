#pragma once
#include "ItemEffect.h"


class HealEffect : public IItemEffect
{
	int Value;
public:
	HealEffect(int value)
	{
		Value = value;
	}
	virtual void Action(IPlayerAbility* Target) override
	{
		Target->Heal(Value);
	}
};
