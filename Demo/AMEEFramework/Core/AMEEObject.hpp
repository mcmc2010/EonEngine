#ifndef __AMEE_OBJECT_H__
#define __AMEE_OBJECT_H__
#pragma once
#include "AMEEGenerateUtils.hpp"
#include <cstdint>
#include <string>

namespace AMEE {

class Object {
public:
    Object(IDType Type = IDType::Node);
    virtual ~Object() = default;

    uint64_t GetID() const { return m_ID; }
    IDType GetIDType() const { return m_Type; }

    const std::string& GetName() const { return m_Name; }
    void SetName(const std::string& Name) { m_Name = Name; }

private:
    IDType m_Type;
    uint64_t m_ID;
    std::string m_Name;
};

} // namespace AMEE

#endif // __AMEE_OBJECT_H__