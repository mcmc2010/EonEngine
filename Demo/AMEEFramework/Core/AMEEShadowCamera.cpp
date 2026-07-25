#include "AMEEShadowCamera.hpp"
#include <cmath>

namespace AMEE {

void ShadowCamera::ConfigureFromLight(const Vec3& LightDir, const Vec3& SceneCenter, float SceneRadius)
{
    // Position: behind the scene center, looking along light direction
    m_Direction = LightDir.Normalized();
    m_Position = SceneCenter - m_Direction * SceneRadius * 2.0f;

    // Handle case where light is pointing straight down
    if (std::abs(m_Direction.y) > 0.99f) {
        m_Up = {1, 0, 0};
    } else {
        m_Up = {0, 1, 0};
    }

    // Set orthographic size based on scene radius
    float Size = SceneRadius * 1.5f;
    m_Left = -Size;
    m_Right = Size;
    m_Bottom = -Size;
    m_Top = Size;
    m_Near = 0.1f;
    m_Far = SceneRadius * 4.0f;
}

Mat4 ShadowCamera::GetViewMatrix() const
{
    return Mat4::LookAt(m_Position, m_Position + m_Direction, m_Up);
}

Mat4 ShadowCamera::GetProjectionMatrix() const
{
    return Mat4::Ortho(m_Left, m_Right, m_Bottom, m_Top, m_Near, m_Far);
}

} // namespace AMEE