#ifndef __AMEE_MACOSWINDOW_H__
#define __AMEE_MACOSWINDOW_H__
#pragma once
#import <Cocoa/Cocoa.h>
#include "../../Core/Platform/IAMEEPlatformWindow.hpp"

namespace AMEE {

class MacosWindow : public IPlatformWindow {
public:
    MacosWindow();
    ~MacosWindow() override;

    bool create(int width, int height, const char* title) override;
    void destroy() override;
    void setSize(int w, int h) override;
    void getSize(int& w, int& h) override;
    void setTitle(const char* title) override;
    void show() override;
    void* getNativeHandle() override;
    bool pollEvents() override;

    void setMinSize(int w, int h) override;
    void center() override;

    NSWindow* getNSWindow() const { return m_pWindow; }

private:
    NSWindow* m_pWindow;
    bool m_ShouldQuit;
};

} // namespace AMEE

#endif // __AMEE_MACOSWINDOW_H__
