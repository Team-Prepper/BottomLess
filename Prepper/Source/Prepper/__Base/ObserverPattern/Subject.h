#pragma once
#include "Observer.h"

template <typename T>
class ISubject
{
public:
    virtual ~ISubject() = default;      // 가상 소멸자
	virtual void Attach(IObserver<T>* Observer) = 0;
	virtual void Detach(IObserver<T>* Observer) = 0;
};