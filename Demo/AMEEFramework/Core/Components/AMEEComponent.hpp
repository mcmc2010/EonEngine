#ifndef __AMEE_COMPONENT_H__
#define __AMEE_COMPONENT_H__
#pragma once

#include "AMEEIComponent.hpp"
#include "../AMEEObject.hpp"

namespace AMEE {

class Entity;

class Component : public Object, public IComponent {
public:
    Component() : Object(ObjectType::Component) {}
    virtual ~Component() = default;

    // IComponent implementation
    Entity* GetOwner() const override { return m_pOwner; }
    void SetOwner(Entity* pOwner) override { m_pOwner = pOwner; }

    bool IsVisible() override { return m_Visible; }
    void SetVisible(bool Value) override { m_Visible = Value; }

    // Lifecycle
    void OnAttach() override {}
    void OnDetach() override {}
    void Update(float DeltaTime) override { (void)DeltaTime; }

protected:
    bool m_Visible = true;

private:
    Entity* m_pOwner = nullptr;
};

} // namespace AMEE

#endif // __AMEE_COMPONENT_H__