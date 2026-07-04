#ifndef __AMEE_OBJECT_H__
#define __AMEE_OBJECT_H__
#pragma once
#include "AMEENode.hpp"
#include <cstdint>
#include <string>

namespace AMEE {

class Object : public Node {
public:
    Object();
    virtual ~Object() = default;

    uint64_t GetID() const { return m_ID; }
    const std::string& GetName() const { return m_Name; }
    void SetName(const std::string& Name) { m_Name = Name; }

private:
    uint64_t m_ID;
    std::string m_Name;
    static uint64_t ms_NextID;
};

} // namespace AMEE

#endif // __AMEE_OBJECT_H__
