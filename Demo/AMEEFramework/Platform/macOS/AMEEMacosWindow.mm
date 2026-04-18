#import "MacWindow.h"
#import "../../Core/Log/AMEELog.h"
#import "../../Core/Platform/IPlatformWindow.h"

MacWindow::MacWindow()
    : _window(nullptr)
    , _shouldQuit(false)
{
}

MacWindow::~MacWindow()
{
    destroy();
}

bool MacWindow::create(int width, int height, const char* title)
{
    NSRect contentRect = NSMakeRect(0, 0, width, height);
    NSUInteger styleMask = NSWindowStyleMaskTitled
        | NSWindowStyleMaskClosable
        | NSWindowStyleMaskMiniaturizable
        | NSWindowStyleMaskResizable;

    _window = [[NSWindow alloc] initWithContentRect:contentRect
                                          styleMask:styleMask
                                            backing:NSBackingStoreBuffered
                                              defer:NO];
    if (!_window) {
        AMEE_LOG_ERROR("MacWindow", "Failed to create NSWindow");
        return false;
    }

    NSString* nsTitle = [NSString stringWithUTF8String:title];
    [_window setTitle:nsTitle];
    [_window setAcceptsMouseMovedEvents:YES];
    [_window setRestorable:NO];

    AMEE_LOG_INFO("MacWindow", "Window created (%dx%d)", width, height);
    return true;
}

void MacWindow::destroy()
{
    if (_window) {
        [_window close];
        _window = nullptr;
    }
}

void MacWindow::setSize(int w, int h)
{
    if (_window) {
        NSSize size = NSMakeSize(w, h);
        [_window setContentSize:size];
    }
}

void MacWindow::getSize(int& w, int& h)
{
    if (_window) {
        NSSize size = [_window frame].size;
        w = (int)size.width;
        h = (int)size.height;
    } else {
        w = h = 0;
    }
}

void MacWindow::setTitle(const char* title)
{
    if (_window) {
        NSString* nsTitle = [NSString stringWithUTF8String:title];
        [_window setTitle:nsTitle];
    }
}

void MacWindow::show()
{
    if (_window) {
        [_window makeKeyAndOrderFront:nil];
    }
}

void* MacWindow::getNativeHandle()
{
    return (__bridge void*)_window;
}

bool MacWindow::pollEvents()
{
    NSEvent* event;
    while ((event = [NSApp nextEventMatchingMask:NSEventMaskAny
                                        untilDate:[NSDate distantPast]
                                           inMode:NSDefaultRunLoopMode
                                          dequeue:YES])) {
        [NSApp sendEvent:event];

        if ([event type] == NSEventTypeApplicationDefined && [event subtype] == 99) {
            _shouldQuit = true;
        }
    }
    return !_shouldQuit;
}

void MacWindow::setMinSize(int w, int h)
{
    if (_window) {
        [_window setMinSize:NSMakeSize(w, h)];
    }
}

void MacWindow::center()
{
    if (_window) {
        [_window center];
    }
}
