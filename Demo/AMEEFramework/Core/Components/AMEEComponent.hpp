#ifndef __AMEE_COMPONENT_H__
#define __AMEE_COMPONENT_H__
#pragma once
#include "../AMEEObject.hpp"

namespace AMEE {

class Entity;

class Component : public Object {
public:
    Component() : Object(ObjectType::Component) {}
    virtual ~Component() = default;

    // Owner
    Entity* GetOwner() const { return m_pOwner; }
    void SetOwner(Entity* pOwner) { m_pOwner = pOwner; }

    // Lifecycle
    virtual void OnAttach() {}
    virtual void OnDetach() {}
    virtual void Update(float DeltaTime) { (void)DeltaTime; }

private:
    Entity* m_pOwner = nullptr;
};

} // namespace AMEE

#endif // __AMEE_COMPONENT_H__