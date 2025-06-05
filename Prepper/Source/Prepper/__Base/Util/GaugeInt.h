// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GaugeValue.h"

/**
 * 
 */
struct FGaugeInt : public GaugeValue<int>
{
private:
	float CurValue;
	float MaxValue;
public:
	FGaugeInt();
	FGaugeInt(int Cur, int Max);
	virtual ~FGaugeInt() override;
	virtual void AddValue(int& Value) override;
	virtual void SubValue(int& Value) override;
	virtual int GetCurValue() const override;
	virtual int GetMaxValue() const override;
	virtual float GetRatio() const override;
	
};

