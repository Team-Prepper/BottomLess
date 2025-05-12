#pragma once
#include "ItemEffect.h"

class EatEffect : public IItemEffect
{
	int Value;
public:
	EatEffect(int value)
	{
		Value = value;
	}
	virtual void Action(IPlayerAbility* Target) override
	{
		Target->Eat(Value);
	} 
};