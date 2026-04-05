#import "GLView.h"
#import <OpenGL/gl.h>
#import <OpenGL/OpenGL.h>

@implementation GLView

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
    NSOpenGLPixelFormat *pixelFormat = [GLView defaultPixelFormat];
    if (!pixelFormat) return nil;

    self = [super initWithFrame:frameRect pixelFormat:pixelFormat];
    if (self)
    {
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
