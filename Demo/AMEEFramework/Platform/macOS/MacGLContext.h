#pragma once
#import <Cocoa/Cocoa.h>
#include "../../Core/Platform/IPlatformGLContext.h"

@class MacGLView;

class MacGLContext : public IPlatformGLContext {
public:
    MacGLContext();
    ~MacGLContext() override;

    bool create(void* windowHandle) override;
    void destroy() override;
    void makeCurrent() override;
    void swapBuffers() override;
    void getSize(int& w, int& h) override;

private:
    MacGLView* _glView;
    NSOpenGLContext* _context;
    NSWindow* _window;
};
