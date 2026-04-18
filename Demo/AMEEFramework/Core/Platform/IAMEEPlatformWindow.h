#pragma once

namespace AMEE {

class IPlatformWindow {
public:
    virtual ~IPlatformWindow() = default;

    virtual bool create(int width, int height, const char* title) = 0;
    virtual void destroy() = 0;
    virtual void setSize(int w, int h) = 0;
    virtual void getSize(int& w, int& h) = 0;
    virtual void setTitle(const char* title) = 0;
    virtual void show() = 0;
    virtual void* getNativeHandle() = 0;
    virtual bool pollEvents() = 0;

    virtual void setMinSize(int w, int h) = 0;
    virtual void center() = 0;
};


} // namespace AMEE