#ifndef __AMEE_MESH_H__
#define __AMEE_MESH_H__
#pragma once
#include "AMEEVertexLayout.hpp"
#include <cstdint>
#include <memory>
#include <vector>
#include <string>

namespace AMEE {

class RHI;

struct SubMesh {
    std::string MaterialName;
    uint32_t IndexStart  = 0;
    uint32_t IndexCount  = 0;
};

class Mesh {
public:
    Mesh() = default;
    ~Mesh();

    bool Create(RHI* rhi, const float* vertices, uint32_t vertexCount,
                const VertexLayout& layout, RHIPrimitive primitive = RHIPrimitive::Triangles);

    bool CreateIndexed(RHI* rhi, const float* vertices, uint32_t vertexCount,
                       const uint32_t* indices, uint32_t indexCount,
                       const VertexLayout& layout, RHIPrimitive primitive = RHIPrimitive::Triangles);

    void Draw() const;
    void DrawSubMesh(int Index) const;

    void Destroy();

    uint32_t GetVAO() const { return m_VAO; }
    uint32_t GetVBO() const { return m_VBO; }
    uint32_t GetEBO() const { return m_EBO; }
    uint32_t GetVertexCount() const { return m_VertexCount; }
    uint32_t GetIndexCount() const { return m_IndexCount; }
    const std::vector<SubMesh>& GetSubMeshes() const { return m_SubMeshes; }
    int GetSubMeshCount() const { return (int)m_SubMeshes.size(); }
    void SetSubMeshes(const std::vector<SubMesh>& Subs) { m_SubMeshes = Subs; }

private:
    RHI* m_pRHI = nullptr;
    uint32_t m_VAO = 0;
    uint32_t m_VBO = 0;
    uint32_t m_EBO = 0;
    uint32_t m_VertexCount = 0;
    uint32_t m_IndexCount = 0;
    RHIPrimitive m_Primitive = RHIPrimitive::Triangles;
    VertexLayout m_Layout;
    std::vector<SubMesh> m_SubMeshes;
};

} // namespace AMEE

#endif // __AMEE_MESH_H__
