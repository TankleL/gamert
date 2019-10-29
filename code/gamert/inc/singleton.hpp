#pragma once

template<class SingletonClass>
class Singleton
{
public:
	static SingletonClass& get_instance()
	{
		static SingletonClass instance;
		return instance;
	}
};

#define DECL_SINGLETON_CTOR(ClassName) friend Singleton; private: ClassName()
