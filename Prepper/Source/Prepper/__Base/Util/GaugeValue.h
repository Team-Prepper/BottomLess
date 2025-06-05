#pragma once

template <typename T>
class GaugeValue
{
public:
	virtual ~GaugeValue() = default;
	virtual void AddValue(T& Value) = 0;
	virtual void SubValue(T& Value) = 0;
	virtual T GetCurValue() const = 0;
	virtual T GetMaxValue() const = 0;
	virtual float GetRatio() const = 0;
};
