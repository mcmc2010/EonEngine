#include "AMEEMesh.hpp"
#include "AMEERHI.hpp"
#include "../Core/Log/AMEELog.hpp"

namespace AMEE {

Mesh::~Mesh()
{
    Destroy();
}

bool Mesh::Create(RHI* rhi, const float* vertices, uint32_t vertexCount,
                  const VertexLayout& layout, RHIPrimitive primitive)
{
    if (!rhi || !vertices || vertexCount == 0) {
        AMEE_LOG_ERROR("Mesh", "Invalid parameters");
        return false;
    }

    Destroy();

    m_pRHI = rhi;
    m_Layout = layout;
    m_Primitive = primitive;
    m_VertexCount = vertexCount;
    m_IndexCount = 0;

    uint32_t dataSize = vertexCount * layout.GetStride();

    // Create VBO
    m_VBO = rhi->createVertexBuffer(vertices, dataSize);
    if (m_VBO == 0) {
        AMEE_LOG_ERROR("Mesh", "Failed to create vertex buffer");
        return false;
    }

    // Create VAO and setup attributes
    m_VAO = rhi->createVertexArray();
    if (m_VAO == 0) {
        AMEE_LOG_ERROR("Mesh", "Failed to create vertex array");
        rhi->destroyVertexBuffer(m_VBO);
        m_VBO = 0;
        return false;
    }

    rhi->bindVertexArray(m_VAO);
    rhi->bindVertexBuffer(m_VBO);

    uint32_t stride = layout.GetStride();
    uint32_t offset = 0;
    for (const auto& attr : layout.GetAttributes()) {
        rhi->enableVertexAttribArray(attr.Index);
        rhi->vertexAttribPointer(attr.Index, attr.ComponentCount, attr.Type,
                                   attr.Normalized, stride, (const void*)(uintptr_t)offset);
        offset += attr.ComponentCount * VertexLayout::GetDataTypeSize(attr.Type);
    }

    rhi->bindVertexArray(0);
    rhi->bindVertexBuffer(0);

    AMEE_LOG_INFO("Mesh", "Created mesh (VAO=%u, VBO=%u, vertices=%u, stride=%u)",
                  m_VAO, m_VBO, vertexCount, stride);
    return true;
}

bool Mesh::CreateIndexed(RHI* rhi, const float* vertices, uint32_t vertexCount,
                          const uint32_t* indices, uint32_t indexCount,
                          const VertexLayout& layout, RHIPrimitive primitive)
{
    if (!rhi || !vertices || vertexCount == 0 || !indices || indexCount == 0) {
        AMEE_LOG_ERROR("Mesh", "Invalid parameters for indexed mesh");
        return false;
    }

    Destroy();

    m_pRHI = rhi;
    m_Layout = layout;
    m_Primitive = primitive;
    m_VertexCount = vertexCount;
    m_IndexCount = indexCount;

    uint32_t vertexDataSize = vertexCount * layout.GetStride();
    uint32_t indexDataSize = indexCount * sizeof(uint32_t);

    // Create VBO
    m_VBO = rhi->createVertexBuffer(vertices, vertexDataSize);
    if (m_VBO == 0) {
        AMEE_LOG_ERROR("Mesh", "Failed to create vertex buffer");
        return false;
    }

    // Create EBO (index buffer)
    m_EBO = rhi->createIndexBuffer(indices, indexDataSize);
    if (m_EBO == 0) {
        AMEE_LOG_ERROR("Mesh", "Failed to create index buffer");
        rhi->destroyVertexBuffer(m_VBO);
        m_VBO = 0;
        return false;
    }

    // Create VAO and setup attributes
    m_VAO = rhi->createVertexArray();
    if (m_VAO == 0) {
        AMEE_LOG_ERROR("Mesh", "Failed to create vertex array");
        rhi->destroyVertexBuffer(m_VBO);
        m_VBO = 0;
        rhi->destroyIndexBuffer(m_EBO);
        m_EBO = 0;
        return false;
    }

    rhi->bindVertexArray(m_VAO);
    rhi->bindVertexBuffer(m_VBO);
    rhi->bindIndexBuffer(m_EBO);

    uint32_t stride = layout.GetStride();
    uint32_t offset = 0;
    for (const auto& attr : layout.GetAttributes()) {
        rhi->enableVertexAttribArray(attr.Index);
        rhi->vertexAttribPointer(attr.Index, attr.ComponentCount, attr.Type,
                                   attr.Normalized, stride, (const void*)(uintptr_t)offset);
        offset += attr.ComponentCount * VertexLayout::GetDataTypeSize(attr.Type);
    }

    rhi->bindVertexArray(0);
    rhi->bindVertexBuffer(0);
    rhi->bindIndexBuffer(0);

    AMEE_LOG_INFO("Mesh", "Created indexed mesh (VAO=%u, VBO=%u, EBO=%u, vertices=%u, indices=%u)",
                  m_VAO, m_VBO, m_EBO, vertexCount, indexCount);
    return true;
}

void Mesh::Draw() const
{
    if (!m_pRHI || m_VAO == 0) return;

    m_pRHI->bindVertexArray(m_VAO);

    if (m_IndexCount > 0) {
        m_pRHI->drawElements(m_Primitive, m_IndexCount, 0);
    } else {
        m_pRHI->drawArrays(m_Primitive, m_VertexCount, 0);
    }

    m_pRHI->bindVertexArray(0);
}

void Mesh::Destroy()
{
    if (m_pRHI) {
        if (m_VAO) {
            m_pRHI->destroyVertexArray(m_VAO);
            m_VAO = 0;
        }
        if (m_VBO) {
            m_pRHI->destroyVertexBuffer(m_VBO);
            m_VBO = 0;
        }
        if (m_EBO) {
            m_pRHI->destroyIndexBuffer(m_EBO);
            m_EBO = 0;
        }
        m_pRHI = nullptr;
    }
    m_VertexCount = 0;
    m_IndexCount = 0;
}

} // namespace AMEE