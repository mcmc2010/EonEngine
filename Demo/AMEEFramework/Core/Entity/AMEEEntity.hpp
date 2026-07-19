#ifndef __AMEE_ENTITY_H__
#define __AMEE_ENTITY_H__
#pragma once
#include "../AMEENode.hpp"
#include "../AMEEObject.hpp"
#include "../Math/AMEEMath.hpp"
#include "../Components/AMEEComponent.hpp"
#include "../AMEEScene.hpp"
#include <vector>
#include <memory>
#include <type_traits>

namespace AMEE {

//
class Entity : public Node, public Object {
public:
    Entity() : Object(ObjectType::Entity) {}
    virtual ~Entity() = default;

    
    // Transform
    Vec3 GetPosition() const { return m_vPosition; }
    void SetPosition(const Vec3& Pos) { m_vPosition = Pos; }

    Vec3 GetRotation() const { return m_vRotation; }
    void SetRotation(const Vec3& Rot) { m_vRotation = Rot; }

    Vec3 GetScale() const { return m_vScale; }
    void SetScale(const Vec3& S) { m_vScale = S; }

    // Matrix
    Mat4 GetLocalMatrix() const;
    Mat4 GetWorldMatrix() const;

    // Direction vectors from rotation
    Vec3 GetForward() const;
    Vec3 GetRight() const;
    Vec3 GetUp() const;

    // Scene
    Scene* GetScene() { return m_pScene; }
    void SetScene(Scene* scene) { m_pScene = scene; }
    
    // Components
    template<typename T, typename... Args>
    T* AddComponent(Args&&... args);

    template<typename T>
    T* GetComponent() const;

    template<typename T>
    std::vector<T*> GetComponents() const;

    bool RemoveComponent(Component* comp);

    const std::vector<std::unique_ptr<Component>>& GetAllComponents() const {
        return m_Components;
    }
    
    // Lifecycle
    virtual void Update(float DeltaTime);

private:
    Vec3 m_vPosition;
    Vec3 m_vRotation = {0, 0, 0};
    Vec3 m_vScale    = {1, 1, 1};

    Scene* m_pScene = nullptr;
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
    m_Components.push_back(std::move(Comp));

    // Notify scene about component addition
    if (m_pScene) {
        m_pScene->OnAddedComponent(this, Ptr);
    }

    // Component is fully initialized, notify it
    Ptr->OnAttach();
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

inline bool Entity::RemoveComponent(Component* comp)
{
    if (!comp) {
        return false;
    }
    
    // Find the component
    for (auto it = m_Components.begin(); it != m_Components.end(); ++it) {
        if (it->get() == comp) {
            // Notify scene before removal
            if (m_pScene) {
                m_pScene->OnRemovedComponent(this, comp);
            }

            // Call OnDetach (like Unity OnDisable)
            comp->OnDetach();

            // Remove from list
            m_Components.erase(it);
            return true;
        }
    }
    return false;
}

} // namespace AMEE

#endif // __AMEE_ENTITY_H__
