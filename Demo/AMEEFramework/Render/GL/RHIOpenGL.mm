#import "RHIOpenGL.h"
#define GL_SILENCE_DEPRECATION
#import <OpenGL/gl.h>
#import <OpenGL/glext.h>
#import <OpenGL/OpenGL.h>
#import <AppKit/AppKit.h>

// macOS uses GL_APPLE_vertex_array_object extension
#define glGenVertexArrays      glGenVertexArraysAPPLE
#define glDeleteVertexArrays   glDeleteVertexArraysAPPLE
#define glBindVertexArray      glBindVertexArrayAPPLE

// Frame
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
    NSOpenGLContext* ctx = NSOpenGLContext.currentContext;
    if (ctx) {
        [ctx flushBuffer];
    }
}

// VBO
uint32_t RHIOpenGL::createVertexBuffer(const float* data, uint32_t size)
{
    GLuint vbo = 0;
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, size, data, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    return vbo;
}

void RHIOpenGL::destroyVertexBuffer(uint32_t id)
{
    GLuint vbo = id;
    glDeleteBuffers(1, &vbo);
}

void RHIOpenGL::bindVertexBuffer(uint32_t id)
{
    glBindBuffer(GL_ARRAY_BUFFER, id);
}

// VAO
uint32_t RHIOpenGL::createVertexArray()
{
    GLuint vao = 0;
    glGenVertexArrays(1, &vao);
    return vao;
}

void RHIOpenGL::destroyVertexArray(uint32_t id)
{
    GLuint vao = id;
    glDeleteVertexArrays(1, &vao);
}

void RHIOpenGL::bindVertexArray(uint32_t id)
{
    glBindVertexArray(id);
}

void RHIOpenGL::vertexAttribPointer(uint32_t index, int size, RHIDataType type, bool normalized, uint32_t stride, const void* pointer)
{
    GLenum glType;
    switch (type) {
        case RHIDataType::Float:         glType = GL_FLOAT; break;
        case RHIDataType::Int:           glType = GL_INT; break;
        case RHIDataType::UnsignedInt:   glType = GL_UNSIGNED_INT; break;
        case RHIDataType::Byte:          glType = GL_BYTE; break;
        case RHIDataType::UnsignedByte:  glType = GL_UNSIGNED_BYTE; break;
    }
    glVertexAttribPointer(index, size, glType, normalized ? GL_TRUE : GL_FALSE, stride, pointer);
}

void RHIOpenGL::enableVertexAttribArray(uint32_t index)
{
    glEnableVertexAttribArray(index);
}

void RHIOpenGL::disableVertexAttribArray(uint32_t index)
{
    glDisableVertexAttribArray(index);
}

// Draw
void RHIOpenGL::drawArrays(RHIPrimitive primitive, uint32_t count, uint32_t offset)
{
    GLenum glPrimitive;
    switch (primitive) {
        case RHIPrimitive::Triangles:      glPrimitive = GL_TRIANGLES; break;
        case RHIPrimitive::TriangleStrip:  glPrimitive = GL_TRIANGLE_STRIP; break;
        case RHIPrimitive::Lines:          glPrimitive = GL_LINES; break;
        case RHIPrimitive::LineStrip:      glPrimitive = GL_LINE_STRIP; break;
        case RHIPrimitive::Points:         glPrimitive = GL_POINTS; break;
    }
    glDrawArrays(glPrimitive, offset, count);
}
