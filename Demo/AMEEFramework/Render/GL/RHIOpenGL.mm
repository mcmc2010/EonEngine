#import "RHIOpenGL.h"
#import <OpenGL/gl.h>
#import <OpenGL/OpenGL.h>
#import <AppKit/AppKit.h>

void RHIOpenGL::setClearColor(float r, float g, float b, float a)
{
    glClearColor(r, g, b, a);
}

void RHIOpenGL::clear()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void RHIOpenGL::setViewport(const RHIViewport& vp)
{
    glViewport(
        static_cast<GLint>(vp.x),
        static_cast<GLint>(vp.y),
        static_cast<GLsizei>(vp.width),
        static_cast<GLsizei>(vp.height)
    );
}

void RHIOpenGL::present()
{
    // macOS: swap buffers via NSOpenGLContext
    [NSOpenGLContext.currentContext flushBuffer];
}
