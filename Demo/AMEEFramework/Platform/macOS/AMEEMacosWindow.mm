#import "AMEEMacosWindow.hpp"
#import "../../Core/Log/AMEELog.hpp"
#import "../../Core/Platform/IAMEEPlatformWindow.hpp"

namespace AMEE {

MacosWindow::MacosWindow()
    : m_pWindow(nullptr)
    , m_ShouldQuit(false)
{
}

MacosWindow::~MacosWindow()
{
    destroy();
}

bool MacosWindow::create(int width, int height, const char* title)
{
    NSRect contentRect = NSMakeRect(0, 0, width, height);
    NSUInteger styleMask = NSWindowStyleMaskTitled
        | NSWindowStyleMaskClosable
        | NSWindowStyleMaskMiniaturizable
        | NSWindowStyleMaskResizable;

    m_pWindow = [[NSWindow alloc] initWithContentRect:contentRect
                                          styleMask:styleMask
                                            backing:NSBackingStoreBuffered
                                              defer:NO];
    if (!m_pWindow) {
        AMEE_LOG_ERROR("MacosWindow", "Failed to create NSWindow");
        return false;
    }

    NSString* nsTitle = [NSString stringWithUTF8String:title];
    [m_pWindow setTitle:nsTitle];
    [m_pWindow setAcceptsMouseMovedEvents:YES];
    [m_pWindow setRestorable:NO];

    AMEE_LOG_INFO("MacosWindow", "Window created (%dx%d)", width, height);
    return true;
}

void MacosWindow::destroy()
{
    if (m_pWindow) {
        [m_pWindow close];
        m_pWindow = nullptr;
    }
}

void MacosWindow::setSize(int w, int h)
{
    if (m_pWindow) {
        NSSize size = NSMakeSize(w, h);
        [m_pWindow setContentSize:size];
    }
}

void MacosWindow::getSize(int& w, int& h)
{
    if (m_pWindow) {
        NSSize size = [m_pWindow frame].size;
        w = (int)size.width;
        h = (int)size.height;
    } else {
        w = h = 0;
    }
}

void MacosWindow::setTitle(const char* title)
{
    if (m_pWindow) {
        NSString* nsTitle = [NSString stringWithUTF8String:title];
        [m_pWindow setTitle:nsTitle];
    }
}

void MacosWindow::show()
{
    if (m_pWindow) {
        [m_pWindow makeKeyAndOrderFront:nil];
    }
}

void* MacosWindow::getNativeHandle()
{
    return (__bridge void*)m_pWindow;
}

bool MacosWindow::pollEvents()
{
    NSEvent* event;
    while ((event = [NSApp nextEventMatchingMask:NSEventMaskAny
                                        untilDate:[NSDate distantPast]
                                           inMode:NSDefaultRunLoopMode
                                          dequeue:YES])) {
        [NSApp sendEvent:event];

        if ([event type] == NSEventTypeApplicationDefined && [event subtype] == 99) {
            m_ShouldQuit = true;
        }
    }
    return !m_ShouldQuit;
}

void MacosWindow::setMinSize(int w, int h)
{
    if (m_pWindow) {
        [m_pWindow setMinSize:NSMakeSize(w, h)];
    }
}

void MacosWindow::center()
{
    if (m_pWindow) {
        [m_pWindow center];
    }
}

} // namespace AMEE
