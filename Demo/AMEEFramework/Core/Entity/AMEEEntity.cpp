#include "AMEEEntity.hpp"

namespace AMEE {

Mat4 Entity::GetLocalMatrix() const
{
    return Mat4::Trs(m_vPosition, m_vRotation, m_vScale);
}

Mat4 Entity::GetWorldMatrix() const
{
    Mat4 Local = GetLocalMatrix();
    if (Node* P = GetParent()) {
        if (Entity* ParentEntity = dynamic_cast<Entity*>(P)) {
            return ParentEntity->GetWorldMatrix() * Local;
        }
    }
    return Local;
}

void Entity::Update(float DeltaTime)
{
    // Update all components
    for (auto& Comp : m_Components) {
        if (Comp && Comp->GetOwner() == this) {
            Comp->Update(DeltaTime);
        }
    }
}

} // namespace AMEE