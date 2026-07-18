#ifndef __AMEE_IPLATFORMCONTEXT_H__
#define __AMEE_IPLATFORMCONTEXT_H__
#pragma once
namespace AMEE {

//
class IPlatformContext {
public:
    virtual ~IPlatformContext() = default;

    virtual bool create(void* windowHandle) = 0;
    virtual void destroy() = 0;
    virtual void makeCurrent() = 0;
    virtual void swapBuffers() = 0;
    virtual void getSize(int& w, int& h) = 0;
};
} // namespace AMEE

#endif // __AMEE_IPLATFORMCONTEXT_H__
