#ifndef __AMEE_ENTITY_H__
#define __AMEE_ENTITY_H__
#pragma once
#include "../AMEENode.hpp"
#include "../AMEEObject.hpp"
#include "../Math/AMEEMath.hpp"

namespace AMEE {

class Entity : public Node, public Object {
public:
    Entity() = default;
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

    // Lifecycle
    virtual void Update(float DeltaTime);

private:
    Vec3 m_vPosition;
    Vec3 m_vRotation = {0, 0, 0};
    Vec3 m_vScale    = {1, 1, 1};
    bool m_Visible   = true;
};

} // namespace AMEE

#endif // __AMEE_ENTITY_H__