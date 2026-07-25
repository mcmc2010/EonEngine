#include "AMEERHIOpenGL.hpp"
#include "../../../Render/Shader/GL/AMEEGLShader.hpp"
#define GL_SILENCE_DEPRECATION
#include <OpenGL/gl3.h>
#include <OpenGL/OpenGL.h>
#import <AppKit/AppKit.h>
#include "AMEEGLCheck.hpp"

namespace AMEE {

// Initialization
void RHIOpenGL::init()
{
    // Enable seamless cubemap sampling (eliminates seam artifacts)
    glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
}

// Frame
void RHIOpenGL::setClearColor(float r, float g, float b, float a)
{
    glClearColor(r, g, b, a);
}

void RHIOpenGL::clear()
{
    GL_CHECK(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));
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

void RHIOpenGL::setDepthMask(bool Write)
{
    glDepthMask(Write ? GL_TRUE : GL_FALSE);
}

void RHIOpenGL::setDepthFunc(bool LessEqual)
{
    glDepthFunc(LessEqual ? GL_LEQUAL : GL_LESS);
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
    GL_CHECK(glGenBuffers(1, &vbo));
    GL_CHECK(glBindBuffer(GL_ARRAY_BUFFER, vbo));
    GL_CHECK(glBufferData(GL_ARRAY_BUFFER, size, data, GL_STATIC_DRAW));
    GL_CHECK(glBindBuffer(GL_ARRAY_BUFFER, 0));
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
    GL_CHECK(glGenBuffers(1, &ebo));
    GL_CHECK(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo));
    GL_CHECK(glBufferData(GL_ELEMENT_ARRAY_BUFFER, size, data, GL_STATIC_DRAW));
    GL_CHECK(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0));
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
    GL_CHECK(glGenVertexArrays(1, &vao));
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
    GL_CHECK(glDrawArrays(glPrimitive, offset, count));
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
    GL_CHECK(glDrawElements(glPrimitive, count, GL_UNSIGNED_INT, (const void*)(uintptr_t)(offset * sizeof(uint32_t))));
}

// Texture
uint32_t RHIOpenGL::createTexture(const unsigned char* data, int width, int height,
                                   RHIFormat format, RHIFormat internalFormat)
{
    GLuint tex = 0;
    GL_CHECK(glGenTextures(1, &tex));
    GL_CHECK(glBindTexture(GL_TEXTURE_2D, tex));

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
    GL_CHECK(glBindTexture(GL_TEXTURE_2D, id));
    GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrapToGL(wrapS)));
    GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrapToGL(wrapT)));
    GL_CHECK(glBindTexture(GL_TEXTURE_2D, 0));
}

// Cubemap Texture
uint32_t RHIOpenGL::createCubemap(const unsigned char* faces[6], int width, int height,
                                   RHIFormat format, RHIFormat internalFormat)
{
    GLuint tex = 0;
    GL_CHECK(glGenTextures(1, &tex));
    GL_CHECK(glBindTexture(GL_TEXTURE_CUBE_MAP, tex));

    GLenum glFormat = formatToGL(format);
    GLenum glInternal = internalFormatToGL(internalFormat);

    GLenum targets[6] = {
        GL_TEXTURE_CUBE_MAP_POSITIVE_X,
        GL_TEXTURE_CUBE_MAP_NEGATIVE_X,
        GL_TEXTURE_CUBE_MAP_POSITIVE_Y,
        GL_TEXTURE_CUBE_MAP_NEGATIVE_Y,
        GL_TEXTURE_CUBE_MAP_POSITIVE_Z,
        GL_TEXTURE_CUBE_MAP_NEGATIVE_Z,
    };

    for (int i = 0; i < 6; i++) {
        GL_CHECK(glTexImage2D(targets[i], 0, glInternal, width, height, 0,
                               glFormat, GL_UNSIGNED_BYTE, faces[i]));
    }

    GL_CHECK(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
    GL_CHECK(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
    GL_CHECK(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
    GL_CHECK(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));
    GL_CHECK(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE));

    GL_CHECK(glBindTexture(GL_TEXTURE_CUBE_MAP, 0));
    return tex;
}

void RHIOpenGL::destroyCubemap(uint32_t id)
{
    GLuint tex = id;
    GL_CHECK(glDeleteTextures(1, &tex));
}

void RHIOpenGL::bindCubemap(uint32_t id, uint32_t slot)
{
    GL_CHECK(glActiveTexture(GL_TEXTURE0 + slot));
    GL_CHECK(glBindTexture(GL_TEXTURE_CUBE_MAP, id));
}

// Framebuffer
RHIFramebuffer RHIOpenGL::createFramebuffer(int width, int height)
{
    RHIFramebuffer fb;
    fb.Width = width;
    fb.Height = height;

    GL_CHECK(glGenFramebuffers(1, &fb.FBO));
    GL_CHECK(glBindFramebuffer(GL_FRAMEBUFFER, fb.FBO));

    GL_CHECK(glGenTextures(1, &fb.DepthTexture));
    GL_CHECK(glBindTexture(GL_TEXTURE_2D, fb.DepthTexture));
    GL_CHECK(glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, width, height, 0,
                           GL_DEPTH_COMPONENT, GL_FLOAT, nullptr));
    GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST));
    GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST));
    GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER));
    GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER));
    float borderColor[] = {1.0f, 1.0f, 1.0f, 1.0f};
    GL_CHECK(glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor));

    GL_CHECK(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
                                     GL_TEXTURE_2D, fb.DepthTexture, 0));
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);

    GL_CHECK(glBindFramebuffer(GL_FRAMEBUFFER, 0));

    return fb;
}

void RHIOpenGL::destroyFramebuffer(RHIFramebuffer& fb)
{
    if (fb.FBO) { GL_CHECK(glDeleteFramebuffers(1, &fb.FBO)); fb.FBO = 0; }
    if (fb.DepthTexture) { GL_CHECK(glDeleteTextures(1, &fb.DepthTexture)); fb.DepthTexture = 0; }
}

void RHIOpenGL::bindFramebuffer(const RHIFramebuffer& fb)
{
    GL_CHECK(glBindFramebuffer(GL_FRAMEBUFFER, fb.FBO));
    GL_CHECK(glViewport(0, 0, fb.Width, fb.Height));
}

void RHIOpenGL::bindDefaultFramebuffer()
{
    GL_CHECK(glBindFramebuffer(GL_FRAMEBUFFER, 0));
}

// Shader
std::unique_ptr<ShaderProgram> RHIOpenGL::CreateShaderProgram()
{
    return std::make_unique<GLShaderProgram>();
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
