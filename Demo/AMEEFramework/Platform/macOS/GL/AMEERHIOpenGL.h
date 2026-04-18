#pragma once
#include "../AMEERHI.h"

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

    // VAO
    uint32_t createVertexArray() override;
    void destroyVertexArray(uint32_t id) override;
    void bindVertexArray(uint32_t id) override;
    void vertexAttribPointer(uint32_t index, int size, RHIDataType type, bool normalized, uint32_t stride, const void* pointer) override;
    void enableVertexAttribArray(uint32_t index) override;
    void disableVertexAttribArray(uint32_t index) override;

    // Draw
    void drawArrays(RHIPrimitive primitive, uint32_t count, uint32_t offset) override;
};
}