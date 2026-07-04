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
    // Default: do nothing, override in subclass
    (void)DeltaTime;
}

} // namespace AMEE