#import "MacGLContext.h"
#define GL_SILENCE_DEPRECATION
#import <OpenGL/gl.h>
#import <OpenGL/OpenGL.h>
#import "../../Core/Log/AMEELog.h"

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

MacGLContext::MacGLContext()
    : _glView(nullptr)
    , _context(nullptr)
    , _window(nullptr)
{
}

MacGLContext::~MacGLContext()
{
    destroy();
}

bool MacGLContext::create(void* windowHandle)
{
    NSWindow* window = (__bridge NSWindow*)windowHandle;
    if (!window) {
        AMEE_LOG_ERROR("MacGLContext", "Invalid window handle");
        return false;
    }

    _window = window;

    NSRect frame = [[window contentView] bounds];
    _glView = [[MacGLView alloc] initWithFrame:frame];
    if (!_glView) {
        AMEE_LOG_ERROR("MacGLContext", "Failed to create OpenGL view");
        return false;
    }

    [window setContentView:_glView];
    [_glView setAutoresizingMask:NSViewWidthSizable | NSViewHeightSizable];

    _context = [_glView openGLContext];
    if (!_context) {
        AMEE_LOG_ERROR("MacGLContext", "Failed to get OpenGL context");
        return false;
    }

    AMEE_LOG_INFO("MacGLContext", "OpenGL 4.1 context created");
    return true;
}

void MacGLContext::destroy()
{
    _glView = nullptr;
    _context = nullptr;
    _window = nullptr;
}

void MacGLContext::makeCurrent()
{
    if (_context) {
        [_context makeCurrentContext];
    }
}

void MacGLContext::swapBuffers()
{
    if (_context) {
        [_context flushBuffer];
    }
}

void MacGLContext::getSize(int& w, int& h)
{
    if (_glView) {
        NSSize size = [_glView drawableSize];
        w = (int)size.width;
        h = (int)size.height;
    } else {
        w = h = 0;
    }
}
