#ifndef __AMEE_MACOSGLCONTEXT_H__
#define __AMEE_MACOSGLCONTEXT_H__
#pragma once
#import <Cocoa/Cocoa.h>
#include "../../Core/Platform/IAMEEPlatformGLContext.hpp"

@class MacGLView;

namespace AMEE {

class MacosGLContext : public IPlatformGLContext {
public:
    MacosGLContext();
    ~MacosGLContext() override;

    bool create(void* windowHandle) override;
    void destroy() override;
    void makeCurrent() override;
    void swapBuffers() override;
    void getSize(int& w, int& h) override;

private:
    MacGLView* m_pGlView;
    NSOpenGLContext* m_pContext;
    NSWindow* m_pWindow;
};

} // namespace AMEE

#endif // __AMEE_MACOSGLCONTEXT_H__
