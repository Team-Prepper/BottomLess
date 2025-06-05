#pragma once

template <typename T>
class USingleton
///*
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
	static void Initialize()
	{
		Instance = new T;
	}
protected:
	static T* Instance;
	USingleton() {}
	
};

template <typename T>
T* USingleton<T>::Instance = nullptr;

//*/{};