#ifndef __AMEE_RHIOPENGL_H__
#define __AMEE_RHIOPENGL_H__
#pragma once
#include "../../../Render/AMEERHI.hpp"
#define GL_SILENCE_DEPRECATION
#include <OpenGL/gl.h>

namespace AMEE {

class RHIOpenGL : public RHI {
public:
    RHIOpenGL() = default;
    ~RHIOpenGL() override = default;

    // Frame
    void setClearColor(float r, float g, float b, float a) override;
    void clear() override;
    void setViewport(const RHIViewport& vp) override;
    void present() override;

    // VBO
    uint32_t createVertexBuffer(const float* data, uint32_t size) override;
    void destroyVertexBuffer(uint32_t id) override;
    void bindVertexBuffer(uint32_t id) override;

    // EBO (Index Buffer)
    uint32_t createIndexBuffer(const uint32_t* data, uint32_t size) override;
    void destroyIndexBuffer(uint32_t id) override;
    void bindIndexBuffer(uint32_t id) override;

    // VAO
    uint32_t createVertexArray() override;
    void destroyVertexArray(uint32_t id) override;
    void bindVertexArray(uint32_t id) override;
    void vertexAttribPointer(uint32_t index, int size, RHIDataType type, bool normalized, uint32_t stride, const void* pointer) override;
    void enableVertexAttribArray(uint32_t index) override;
    void disableVertexAttribArray(uint32_t index) override;

    // Texture
    uint32_t createTexture(const unsigned char* data, int width, int height,
                            RHIFormat format, RHIFormat internalFormat) override;
    void destroyTexture(uint32_t id) override;
    void bindTexture(uint32_t id, uint32_t slot) override;
    void setTextureFilter(uint32_t id, RHIFilter minFilter, RHIFilter magFilter) override;
    void setTextureWrap(uint32_t id, RHIWrap wrapS, RHIWrap wrapT) override;

    // Draw
    void drawArrays(RHIPrimitive primitive, uint32_t count, uint32_t offset) override;
    void drawElements(RHIPrimitive primitive, uint32_t count, uint32_t offset) override;

private:
    static GLenum formatToGL(RHIFormat format);
    static GLenum internalFormatToGL(RHIFormat format);
    static GLenum filterToGL(RHIFilter filter);
    static GLenum wrapToGL(RHIWrap wrap);
};
}

#endif // __AMEE_RHIOPENGL_H__