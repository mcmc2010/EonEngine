#ifndef __AMEE_OBJECT_H__
#define __AMEE_OBJECT_H__
#pragma once

#include "AMEEDefines.hpp"
#include "AMEEGenerateUtils.hpp"
#include "AMEELayerMask.hpp"
#include <cstdint>
#include <string>

namespace AMEE {

enum class ObjectType : uint8_t {
    Node      = 0,
    Entity    = 1,
    Component = 2,
    Resource  = 100,
};

class Object {
public:
    Object(ObjectType Type = ObjectType::Node);
    virtual ~Object() = default;

    uint64_t GetID() const { return m_ID; }
    ObjectType GetType() const { return m_Type; }
    IDType GetIDType() const { return IDGenerator::GetType(m_ID); }

    const std::string& GetName() const { return m_Name; }
    void SetName(const std::string& Name) { m_Name = Name; }

    const std::string& GetTag() const { return m_Tag; }
    void SetTag(const std::string& Tag) { m_Tag = Tag; }

    LayerMask GetLayer() const { return m_Layer; }
    void SetLayer(LayerMask Layer) { m_Layer = Layer; }
    void SetLayer(Layer Layer) { m_Layer = LayerMask(Layer); }

private:
    static IDType ToIDType(ObjectType Type);

    ObjectType m_Type;
    uint64_t m_ID;

    std::string m_Name;
    std::string m_Tag = TAG_UNTAGGED;
    LayerMask m_Layer = LayerMask::Default();
};

} // namespace AMEE

#endif // __AMEE_OBJECT_H__