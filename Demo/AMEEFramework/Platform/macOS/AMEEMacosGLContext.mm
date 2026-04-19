#import "AMEEMacosGLContext.hpp"
#define GL_SILENCE_DEPRECATION
#import <OpenGL/gl.h>
#import <OpenGL/OpenGL.h>
#import "../../Core/Log/AMEELog.hpp"
#include "../../Core/Platform/IAMEEPlatformGLContext.hpp"

@interface MacGLView : NSOpenGLView
@end

@implementation MacGLView
+ (NSOpenGLPixelFormat *)defaultPixelFormat
{
    NSOpenGLPixelFormatAttribute attrs[] = {
        NSOpenGLPFAOpenGLProfile, NSOpenGLProfileVersion4_1Core,
        NSOpenGLPFAColorSize, 24,
        NSOpenGLPFADepthSize, 24,
        NSOpenGLPFAStencilSize, 8,
        NSOpenGLPFADoubleBuffer, YES,
        NSOpenGLPFAAccelerated, YES,
        0
    };
    return [[NSOpenGLPixelFormat alloc] initWithAttributes:attrs];
}

- (instancetype)initWithFrame:(NSRect)frameRect
{
    NSOpenGLPixelFormat *pixelFormat = [MacGLView defaultPixelFormat];
    if (!pixelFormat) return nil;

    self = [super initWithFrame:frameRect pixelFormat:pixelFormat];
    if (self) {
        [self setWantsBestResolutionOpenGLSurface:YES];
        [[self openGLContext] makeCurrentContext];
        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LEQUAL);
    }
    return self;
}

- (NSSize)drawableSize
{
    return [self convertSizeToBacking:self.bounds.size];
}

- (void)makeGLContextCurrent
{
    [[self openGLContext] makeCurrentContext];
}
@end

namespace AMEE {

MacosGLContext::MacosGLContext()
    : m_pGlView(nullptr)
    , m_pContext(nullptr)
    , m_pWindow(nullptr)
{
}

MacosGLContext::~MacosGLContext()
{
    destroy();
}

bool MacosGLContext::create(void* windowHandle)
{
    NSWindow* window = (__bridge NSWindow*)windowHandle;
    if (!window) {
        AMEE_LOG_ERROR("MacosGLContext", "Invalid window handle");
        return false;
    }

    m_pWindow = window;

    NSRect frame = [[window contentView] bounds];
    m_pGlView = [[MacGLView alloc] initWithFrame:frame];
    if (!m_pGlView) {
        AMEE_LOG_ERROR("MacosGLContext", "Failed to create OpenGL view");
        return false;
    }

    [window setContentView:m_pGlView];
    [m_pGlView setAutoresizingMask:NSViewWidthSizable | NSViewHeightSizable];

    m_pContext = [m_pGlView openGLContext];
    if (!m_pContext) {
        AMEE_LOG_ERROR("MacosGLContext", "Failed to get OpenGL context");
        return false;
    }

    AMEE_LOG_INFO("MacosGLContext", "OpenGL 4.1 context created");
    return true;
}

void MacosGLContext::destroy()
{
    m_pGlView = nullptr;
    m_pContext = nullptr;
    m_pWindow = nullptr;
}

void MacosGLContext::makeCurrent()
{
    if (m_pContext) {
        [m_pContext makeCurrentContext];
    }
}

void MacosGLContext::swapBuffers()
{
    if (m_pContext) {
        [m_pContext flushBuffer];
    }
}

void MacosGLContext::getSize(int& w, int& h)
{
    if (m_pGlView) {
        NSSize size = [m_pGlView drawableSize];
        w = (int)size.width;
        h = (int)size.height;
    } else {
        w = h = 0;
    }
}

} // namespace AMEE
