#pragma once
#include "Prepper/_Base/Util/GaugeFloat.h"

class State
{
public:
	State(const FGaugeFloat& Hungry, const FGaugeFloat& Thirsty, const FGaugeFloat& Infected);
	FGaugeFloat Hungry;
	FGaugeFloat Thirsty;
	FGaugeFloat Infected;
};
