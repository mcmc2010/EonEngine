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
        assert(!ms_Singleton);
        ms_Singleton = static_cast<T*>(this);
    }

    virtual ~SingletonT()
    {
        assert(ms_Singleton);
        ms_Singleton = nullptr;
    }

    static T& GetSingleton()
    {
        assert(ms_Singleton);
        return (*ms_Singleton);
    }

    static T* GetSingletonPtr()
    {
        return ms_Singleton;
    }

protected:
    inline static T* ms_Singleton = nullptr;
};

// 宏保留用于兼容，但不再需要
#define AMEE_SINGLETON_IMPL(T)

} // namespace AMEE

#endif // __AMEE_SINGLETON_H__