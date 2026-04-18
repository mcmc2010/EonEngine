#pragma once
#import <Cocoa/Cocoa.h>
#include "../../Core/Platform/IPlatformWindow.h"

class MacWindow : public IPlatformWindow {
public:
    MacWindow();
    ~MacWindow() override;

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

    NSWindow* getNSWindow() const { return _window; }

private:
    NSWindow* _window;
    bool _shouldQuit;
};
