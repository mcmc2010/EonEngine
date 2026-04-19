#ifndef __AMEE_RHI_H__
#define __AMEE_RHI_H__
#pragma once
#include "AMEERHITypes.hpp"
#include <cstdint>

namespace AMEE {


// Primitives
enum class RHIPrimitive : uint8_t {
    Triangles,
    TriangleStrip,
    Lines,
    LineStrip,
    Points,
};

// Data types
enum class RHIDataType : uint8_t {
    Float,
    Int,
    UnsignedInt,
    Byte,
    UnsignedByte,
};

// Draw command
struct RHIDrawCommand {
    RHIPrimitive primitive = RHIPrimitive::Triangles;
    uint32_t vertexCount = 0;
    uint32_t offset = 0;
};

class RHI {
public:
    virtual ~RHI() = default;

    // Frame
    virtual void setClearColor(float r, float g, float b, float a) = 0;
    virtual void clear() = 0;
    virtual void setViewport(const RHIViewport& vp) = 0;
    virtual void present() = 0;

    // VBO
    virtual uint32_t createVertexBuffer(const float* data, uint32_t size) = 0;
    virtual void destroyVertexBuffer(uint32_t id) = 0;
    virtual void bindVertexBuffer(uint32_t id) = 0;

    // VAO
    virtual uint32_t createVertexArray() = 0;
    virtual void destroyVertexArray(uint32_t id) = 0;
    virtual void bindVertexArray(uint32_t id) = 0;
    virtual void vertexAttribPointer(uint32_t index, int size, RHIDataType type, bool normalized, uint32_t stride, const void* pointer) = 0;
    virtual void enableVertexAttribArray(uint32_t index) = 0;
    virtual void disableVertexAttribArray(uint32_t index) = 0;

    // Draw
    virtual void drawArrays(RHIPrimitive primitive, uint32_t count, uint32_t offset = 0) = 0;
};
}

#endif // __AMEE_RHI_H__