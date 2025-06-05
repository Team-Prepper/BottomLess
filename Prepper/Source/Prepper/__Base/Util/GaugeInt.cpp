#include "GaugeInt.h"

FGaugeInt::FGaugeInt()
{
	CurValue = 0;
	MaxValue = 0;
}

FGaugeInt::FGaugeInt(int Cur, int Max)
{
	CurValue = Cur;
	MaxValue = Max;
}

FGaugeInt::~FGaugeInt()
{
}

void FGaugeInt::AddValue(int& Value)
{
	CurValue += Value;
	if (CurValue > MaxValue) CurValue = MaxValue;
}

void FGaugeInt::SubValue(int& Value)
{
	CurValue -= Value;
	if (CurValue < 0) CurValue = 0;
}

int FGaugeInt::GetCurValue() const
{
	return CurValue;
}

int FGaugeInt::GetMaxValue() const
{
	return MaxValue;
}

float FGaugeInt::GetRatio() const
{
	return CurValue / static_cast<float>(MaxValue);
}