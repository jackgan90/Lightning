#pragma once

template<typename T>
class Singleton
{
public:
	static T* Instance()
	{
		static T instance;
		return &instance;
	}
protected:
	Singleton(){}
	virtual ~Singleton(){}
private:
	Singleton(const Singleton<T>&) = delete;
	Singleton<T>& operator=(const Singleton<T>&) = delete;
};
