#include "AMEERHIOpenGL.hpp"
#define GL_SILENCE_DEPRECATION
#include <OpenGL/gl.h>
#include <OpenGL/glext.h>
#include <OpenGL/OpenGL.h>
#import <AppKit/AppKit.h>

// macOS uses GL_APPLE_vertex_array_object extension
#define glGenVertexArrays      glGenVertexArraysAPPLE
#define glDeleteVertexArrays   glDeleteVertexArraysAPPLE
#define glBindVertexArray      glBindVertexArrayAPPLE

namespace AMEE {

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

// EBO (Index Buffer)
uint32_t RHIOpenGL::createIndexBuffer(const uint32_t* data, uint32_t size)
{
    GLuint ebo = 0;
    glGenBuffers(1, &ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, size, data, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    return ebo;
}

void RHIOpenGL::destroyIndexBuffer(uint32_t id)
{
    GLuint ebo = id;
    glDeleteBuffers(1, &ebo);
}

void RHIOpenGL::bindIndexBuffer(uint32_t id)
{
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, id);
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

void RHIOpenGL::drawElements(RHIPrimitive primitive, uint32_t count, uint32_t offset)
{
    GLenum glPrimitive;
    switch (primitive) {
        case RHIPrimitive::Triangles:      glPrimitive = GL_TRIANGLES; break;
        case RHIPrimitive::TriangleStrip:  glPrimitive = GL_TRIANGLE_STRIP; break;
        case RHIPrimitive::Lines:          glPrimitive = GL_LINES; break;
        case RHIPrimitive::LineStrip:      glPrimitive = GL_LINE_STRIP; break;
        case RHIPrimitive::Points:         glPrimitive = GL_POINTS; break;
    }
    glDrawElements(glPrimitive, count, GL_UNSIGNED_INT, (const void*)(uintptr_t)(offset * sizeof(uint32_t)));
}

// Texture
uint32_t RHIOpenGL::createTexture(const unsigned char* data, int width, int height,
                                   RHIFormat format, RHIFormat internalFormat)
{
    GLuint tex = 0;
    glGenTextures(1, &tex);
    glBindTexture(GL_TEXTURE_2D, tex);

    GLenum glFormat = formatToGL(format);
    GLenum glInternal = internalFormatToGL(internalFormat);

    glTexImage2D(GL_TEXTURE_2D, 0, glInternal, width, height, 0,
                 glFormat, GL_UNSIGNED_BYTE, data);

    // Default filtering
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    glBindTexture(GL_TEXTURE_2D, 0);
    return tex;
}

void RHIOpenGL::destroyTexture(uint32_t id)
{
    GLuint tex = id;
    glDeleteTextures(1, &tex);
}

void RHIOpenGL::bindTexture(uint32_t id, uint32_t slot)
{
    glActiveTexture(GL_TEXTURE0 + slot);
    glBindTexture(GL_TEXTURE_2D, id);
}

void RHIOpenGL::setTextureFilter(uint32_t id, RHIFilter minFilter, RHIFilter magFilter)
{
    glBindTexture(GL_TEXTURE_2D, id);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filterToGL(minFilter));
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filterToGL(magFilter));
    glBindTexture(GL_TEXTURE_2D, 0);
}

void RHIOpenGL::setTextureWrap(uint32_t id, RHIWrap wrapS, RHIWrap wrapT)
{
    glBindTexture(GL_TEXTURE_2D, id);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrapToGL(wrapS));
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrapToGL(wrapT));
    glBindTexture(GL_TEXTURE_2D, 0);
}

// Helper methods
GLenum RHIOpenGL::formatToGL(RHIFormat format)
{
    switch (format) {
        case RHIFormat::R8:    return GL_RED;
        case RHIFormat::RG8:   return GL_RG;
        case RHIFormat::RGB8:  return GL_RGB;
        case RHIFormat::RGBA8: return GL_RGBA;
    }
    return GL_RGBA;
}

GLenum RHIOpenGL::internalFormatToGL(RHIFormat format)
{
    switch (format) {
        case RHIFormat::R8:    return GL_R8;
        case RHIFormat::RG8:   return GL_RG8;
        case RHIFormat::RGB8:  return GL_RGB8;
        case RHIFormat::RGBA8: return GL_RGBA8;
    }
    return GL_RGBA8;
}

GLenum RHIOpenGL::filterToGL(RHIFilter filter)
{
    switch (filter) {
        case RHIFilter::Nearest: return GL_NEAREST;
        case RHIFilter::Linear:  return GL_LINEAR;
    }
    return GL_LINEAR;
}

GLenum RHIOpenGL::wrapToGL(RHIWrap wrap)
{
    switch (wrap) {
        case RHIWrap::Repeat:         return GL_REPEAT;
        case RHIWrap::ClampToEdge:    return GL_CLAMP_TO_EDGE;
        case RHIWrap::MirroredRepeat: return GL_MIRRORED_REPEAT;
    }
    return GL_REPEAT;
}
}