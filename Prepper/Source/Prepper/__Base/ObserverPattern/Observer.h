#pragma once

template <typename T>
class IObserver {
public:
    virtual ~IObserver() = default;      // 가상 소멸자
    virtual void Update(const T& NewData) = 0; // 순수 가상 함수
};