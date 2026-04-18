#pragma once

class IPlatformGLContext {
public:
    virtual ~IPlatformGLContext() = default;

    virtual bool create(void* windowHandle) = 0;
    virtual void destroy() = 0;
    virtual void makeCurrent() = 0;
    virtual void swapBuffers() = 0;
    virtual void getSize(int& w, int& h) = 0;
};
