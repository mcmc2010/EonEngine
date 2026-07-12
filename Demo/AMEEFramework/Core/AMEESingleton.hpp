#ifndef __AMEE_SINGLETON_H__
#define __AMEE_SINGLETON_H__
#pragma once

namespace AMEE {

template<typename T>
class Singleton {
public:
    static T& Instance()
    {
        static T instance;
        return instance;
    }

    Singleton(const Singleton&) = delete;
    Singleton& operator=(const Singleton&) = delete;
    Singleton(Singleton&&) = delete;
    Singleton& operator=(Singleton&&) = delete;

protected:
    Singleton() = default;
    virtual ~Singleton() = default;
};

} // namespace AMEE

#endif // __AMEE_SINGLETON_H__