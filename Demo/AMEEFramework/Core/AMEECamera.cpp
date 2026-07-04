#include "AMEECamera.hpp"
#include <cmath>

namespace AMEE {

Camera::Camera(float FovDeg, float Near, float Far)
    : m_Position(0, 0, 0)
    , m_Yaw(-90.0f)
    , m_Pitch(0)
    , m_FovDeg(FovDeg)
    , m_Near(Near)
    , m_Far(Far)
{
}

void Camera::SetPosition(const Vec3& Pos)
{
    m_Position = Pos;
}

void Camera::SetRotation(float Yaw, float Pitch)
{
    m_Yaw = Yaw;
    m_Pitch = Pitch;
}

void Camera::Move(const Vec3& Offset)
{
    m_Position = m_Position + Offset;
}

void Camera::MoveForward(float Amount)
{
    m_Position = m_Position + GetForward() * Amount;
}

void Camera::MoveRight(float Amount)
{
    m_Position = m_Position + GetRight() * Amount;
}

void Camera::MoveUp(float Amount)
{
    m_Position = m_Position + Vec3(0, Amount, 0);
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
    return Mat4::LookAt(m_Position, m_Position + GetForward(), Vec3(0, 1, 0));
}

Mat4 Camera::GetProjectionMatrix(float Aspect) const
{
    return Mat4::Perspective(m_FovDeg, Aspect, m_Near, m_Far);
}

Vec3 Camera::GetForward() const
{
    float yawRad = m_Yaw * DEG2RAD;
    float pitchRad = m_Pitch * DEG2RAD;
    return {
        std::cos(pitchRad) * std::cos(yawRad),
        std::sin(pitchRad),
        std::cos(pitchRad) * std::sin(yawRad)
    };
}

Vec3 Camera::GetRight() const
{
    float yawRad = m_Yaw * DEG2RAD;
    return {
        std::sin(yawRad),
        0,
        -std::cos(yawRad)
    };
}

Vec3 Camera::GetUp() const
{
    return Vec3::Cross(GetRight(), GetForward());
}

} // namespace AMEE
