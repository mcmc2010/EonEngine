#ifndef __AMEE_MESH_H__
#define __AMEE_MESH_H__
#pragma once
#include "AMEEVertexLayout.hpp"
#include <cstdint>
#include <memory>

namespace AMEE {

class RHI;

class Mesh {
public:
    Mesh() = default;
    ~Mesh();

    // Create mesh from vertex data + layout
    bool Create(RHI* rhi, const float* vertices, uint32_t vertexCount,
                const VertexLayout& layout, RHIPrimitive primitive = RHIPrimitive::Triangles);

    // Create indexed mesh
    bool CreateIndexed(RHI* rhi, const float* vertices, uint32_t vertexCount,
                       const uint32_t* indices, uint32_t indexCount,
                       const VertexLayout& layout, RHIPrimitive primitive = RHIPrimitive::Triangles);

    // Bind and draw
    void Draw() const;

    // Cleanup
    void Destroy();

    // Accessors
    uint32_t GetVAO() const { return m_VAO; }
    uint32_t GetVBO() const { return m_VBO; }
    uint32_t GetEBO() const { return m_EBO; }
    uint32_t GetVertexCount() const { return m_VertexCount; }
    uint32_t GetIndexCount() const { return m_IndexCount; }

private:
    RHI* m_pRHI = nullptr;
    uint32_t m_VAO = 0;
    uint32_t m_VBO = 0;
    uint32_t m_EBO = 0;
    uint32_t m_VertexCount = 0;
    uint32_t m_IndexCount = 0;
    RHIPrimitive m_Primitive = RHIPrimitive::Triangles;
    VertexLayout m_Layout;
};

} // namespace AMEE

#endif // __AMEE_MESH_H__
