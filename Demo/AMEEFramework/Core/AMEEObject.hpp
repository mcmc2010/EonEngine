#ifndef __AMEE_OBJECT_H__
#define __AMEE_OBJECT_H__
#pragma once
#include "AMEEGenerateUtils.hpp"
#include <cstdint>
#include <string>

namespace AMEE {

enum class ObjectType {
    Node      = 0,
    Entity    = 1,
    Component = 2,
    //
    Resoursce = 100,
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

private:
    static IDType ToIDType(ObjectType Type);

    ObjectType m_Type;
    uint64_t m_ID;
    std::string m_Name;
};

} // namespace AMEE

#endif // __AMEE_OBJECT_H__
