// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GaugeValue.h"

/**
 * 
 */
struct FGaugeFloat : public GaugeValue<float>
{
private:
	float CurValue;
	float MaxValue;
public:
	FGaugeFloat();
	FGaugeFloat(float Cur, float Max);
	virtual ~FGaugeFloat() override;
	virtual void AddValue(float& Value) override;
	virtual void SubValue(float& Value) override;
	virtual float GetCurValue() const override;
	virtual float GetMaxValue() const override;
	virtual float GetRatio() const override;
	
};

