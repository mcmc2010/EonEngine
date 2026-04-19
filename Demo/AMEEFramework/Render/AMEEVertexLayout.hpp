#ifndef __AMEE_VERTEXLAYOUT_H__
#define __AMEE_VERTEXLAYOUT_H__
#pragma once
#include "../Render/AMEERHI.hpp"
#include <vector>
#include <cstdint>

namespace AMEE {

struct VertexAttribute {
    uint32_t Index;         // Attribute location (0, 1, 2, ...)
    int ComponentCount;     // Number of components (1, 2, 3, 4)
    RHIDataType Type;       // Data type
    bool Normalized;        // Should normalize?
};

class VertexLayout {
public:
    VertexLayout() = default;

    VertexLayout& Add(uint32_t index, int componentCount, RHIDataType type, bool normalized = false)
    {
        m_Attributes.push_back({index, componentCount, type, normalized});
        return *this;
    }

    uint32_t GetStride() const
    {
        uint32_t stride = 0;
        for (const auto& attr : m_Attributes) {
            stride += attr.ComponentCount * GetDataTypeSize(attr.Type);
        }
        return stride;
    }

    const std::vector<VertexAttribute>& GetAttributes() const { return m_Attributes; }

    static uint32_t GetDataTypeSize(RHIDataType type)
    {
        switch (type) {
            case RHIDataType::Float:         return 4;
            case RHIDataType::Int:           return 4;
            case RHIDataType::UnsignedInt:   return 4;
            case RHIDataType::Byte:          return 1;
            case RHIDataType::UnsignedByte:  return 1;
        }
        return 4;
    }

private:
    std::vector<VertexAttribute> m_Attributes;
};

} // namespace AMEE

#endif // __AMEE_VERTEXLAYOUT_H__
