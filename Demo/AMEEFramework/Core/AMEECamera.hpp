#ifndef __AMEE_CAMERA_H__
#define __AMEE_CAMERA_H__
#pragma once
#include "Math/AMEEMath.hpp"

namespace AMEE {

class Camera {
public:
    Camera(float FovDeg = 60.0f, float Near = 0.1f, float Far = 1000.0f);

    void SetPosition(const Vec3& Pos);
    void SetRotation(float Yaw, float Pitch);

    Vec3 GetPosition() const { return m_Position; }
    float GetYaw() const { return m_Yaw; }
    float GetPitch() const { return m_Pitch; }

    void Move(const Vec3& Offset);
    void MoveForward(float Amount);
    void MoveRight(float Amount);
    void MoveUp(float Amount);
    void Rotate(float YawDelta, float PitchDelta);

    Mat4 GetViewMatrix() const;
    Mat4 GetProjectionMatrix(float Aspect) const;

    Vec3 GetForward() const;
    Vec3 GetRight() const;
    Vec3 GetUp() const;

private:
    Vec3 m_Position;
    float m_Yaw;
    float m_Pitch;
    float m_FovDeg;
    float m_Near;
    float m_Far;
};

} // namespace AMEE

#endif // __AMEE_CAMERA_H__
