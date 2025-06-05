#pragma once

template <typename T>
class Singleton
{
public:
	static T* GetInstance()
	{
		if (Instance == nullptr)
		{
			Instance = new T;
		}
		return Instance;
	}
	Singleton() {}
	virtual ~Singleton() {}
	virtual void OnCreate() {}
protected:
	static T* Instance;
public:
	Singleton(Singleton const &) = delete;
	Singleton& operator=(Singleton const &) = delete;
};

template <typename T> T* Singleton<T>::Instance = nullptr;