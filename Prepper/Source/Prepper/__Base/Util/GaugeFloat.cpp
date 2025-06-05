// Fill out your copyright notice in the Description page of Project Settings.


#include "GaugeFloat.h"

#include "Curves/CurveEvaluation.h"

FGaugeFloat::FGaugeFloat()
{
	CurValue = 0;
	MaxValue = 0;
}

FGaugeFloat::FGaugeFloat(float Cur, float Max)
{
	CurValue = Cur;
	MaxValue = Max;
}

FGaugeFloat::~FGaugeFloat()
{
}

void FGaugeFloat::AddValue(float& Value)
{
	CurValue += Value;
	if (CurValue > MaxValue) CurValue = MaxValue;
}

void FGaugeFloat::SubValue(float& Value)
{
	CurValue -= Value;
	if (CurValue < 0) CurValue = 0;
}

float FGaugeFloat::GetCurValue() const
{
	return CurValue;
}

float FGaugeFloat::GetMaxValue() const
{
	return MaxValue;
}

float FGaugeFloat::GetRatio() const
{
	return CurValue / MaxValue;
}