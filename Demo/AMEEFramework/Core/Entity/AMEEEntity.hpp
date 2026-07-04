#ifndef __AMEE_ENTITY_H__
#define __AMEE_ENTITY_H__
#pragma once
#include "../AMEENode.hpp"
#include "../AMEEObject.hpp"
#include "../Math/AMEEMath.hpp"
#include "../Components/AMEEComponent.hpp"
#include <vector>
#include <memory>
#include <type_traits>

namespace AMEE {

class Entity : public Node, public Object {
public:
    Entity() : Object(IDType::Entity) {}
    virtual ~Entity() = default;

    // Transform
    Vec3 GetPosition() const { return m_vPosition; }
    void SetPosition(const Vec3& Pos) { m_vPosition = Pos; }

    Vec3 GetRotation() const { return m_vRotation; }
    void SetRotation(const Vec3& Rot) { m_vRotation = Rot; }

    Vec3 GetScale() const { return m_vScale; }
    void SetScale(const Vec3& S) { m_vScale = S; }

    bool IsVisible() const { return m_Visible; }
    void SetVisible(bool Visible) { m_Visible = Visible; }

    // Matrix
    Mat4 GetLocalMatrix() const;
    Mat4 GetWorldMatrix() const;

    // Components
    template<typename T, typename... Args>
    T* AddComponent(Args&&... args);

    template<typename T>
    T* GetComponent() const;

    template<typename T>
    std::vector<T*> GetComponents() const;

    // Lifecycle
    virtual void Update(float DeltaTime);

private:
    Vec3 m_vPosition;
    Vec3 m_vRotation = {0, 0, 0};
    Vec3 m_vScale    = {1, 1, 1};
    bool m_Visible   = true;

    std::vector<std::unique_ptr<Component>> m_Components;
};

// Template implementations

template<typename T, typename... Args>
T* Entity::AddComponent(Args&&... args)
{
    static_assert(std::is_base_of<Component, T>::value, "T must derive from Component");
    auto Comp = std::make_unique<T>(std::forward<Args>(args)...);
    T* Ptr = Comp.get();
    Comp->SetOwner(this);
    Comp->OnAttach();
    m_Components.push_back(std::move(Comp));
    return Ptr;
}

template<typename T>
T* Entity::GetComponent() const
{
    static_assert(std::is_base_of<Component, T>::value, "T must derive from Component");
    for (const auto& Comp : m_Components) {
        if (T* Ptr = dynamic_cast<T*>(Comp.get())) {
            return Ptr;
        }
    }
    return nullptr;
}

template<typename T>
std::vector<T*> Entity::GetComponents() const
{
    static_assert(std::is_base_of<Component, T>::value, "T must derive from Component");
    std::vector<T*> Result;
    for (const auto& Comp : m_Components) {
        if (T* Ptr = dynamic_cast<T*>(Comp.get())) {
            Result.push_back(Ptr);
        }
    }
    return Result;
}

} // namespace AMEE

#endif // __AMEE_ENTITY_H__