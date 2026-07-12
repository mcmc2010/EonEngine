#ifndef __AMEE_SINGLETON_H__
#define __AMEE_SINGLETON_H__
#pragma once
#include <cassert>

namespace AMEE {

template<typename T>
class SingletonT
{
public:
    SingletonT()
    {
        assert(ms_Singleton == nullptr);
        ms_Singleton = static_cast<T*>(this);
    }

    virtual ~SingletonT()
    {
        assert(ms_Singleton == nullptr);
        ms_Singleton = nullptr;
    }
    


    static T& GetSingleton()
    {
        if(ms_Singleton == nullptr)
        {
            ms_Singleton = NewInstance();
        }
        return (*ms_Singleton);
    }

    static T* GetSingletonPtr()
    {
        if(ms_Singleton == nullptr)
        {
            ms_Singleton = NewInstance();
        }
        return ms_Singleton;
    }

protected:
    static T* NewInstance()
    {
        assert(ms_Singleton == nullptr);
        return (ms_Singleton = new T());
    }
    
    inline static T* ms_Singleton;
};

// 宏保留用于兼容，但不再需要
#define AMEE_SINGLETON_IMPL(_Ty) template<>  _Ty* SingletonT<_Ty>::ms_Singleton = nullptr;

} // namespace AMEE

#endif // __AMEE_SINGLETON_H__
