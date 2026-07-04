#include "AMEECamera.hpp"
#include "Entity/AMEEEntity.hpp"
#include <cmath>

namespace AMEE {

Camera::Camera(float FovDeg, float Near, float Far)
    : m_FovDeg(FovDeg)
    , m_Near(Near)
    , m_Far(Far)
{
}

void Camera::SetRotation(float Yaw, float Pitch)
{
    m_Yaw = Yaw;
    m_Pitch = Pitch;
    if (m_Pitch > 89.0f) m_Pitch = 89.0f;
    if (m_Pitch < -89.0f) m_Pitch = -89.0f;
}

void Camera::Rotate(float YawDelta, float PitchDelta)
{
    m_Yaw += YawDelta;
    m_Pitch += PitchDelta;
    if (m_Pitch > 89.0f) m_Pitch = 89.0f;
    if (m_Pitch < -89.0f) m_Pitch = -89.0f;
}

Mat4 Camera::GetViewMatrix() const
{
    Vec3 Pos = GetOwner() ? GetOwner()->GetPosition() : Vec3(0);
    return Mat4::LookAt(Pos, Pos + GetForward(), Vec3(0, 1, 0));
}

Mat4 Camera::GetProjectionMatrix(float Aspect) const
{
    return Mat4::Perspective(m_FovDeg, Aspect, m_Near, m_Far);
}

Vec3 Camera::GetForward() const
{
    float YawRad = m_Yaw * DEG2RAD;
    float PitchRad = m_Pitch * DEG2RAD;
    return {
        std::cos(PitchRad) * std::cos(YawRad),
        std::sin(PitchRad),
        std::cos(PitchRad) * std::sin(YawRad)
    };
}

Vec3 Camera::GetRight() const
{
    float YawRad = m_Yaw * DEG2RAD;
    return {
        std::sin(YawRad),
        0,
        -std::cos(YawRad)
    };
}

Vec3 Camera::GetUp() const
{
    return Vec3::Cross(GetRight(), GetForward());
}

} // namespace AMEE