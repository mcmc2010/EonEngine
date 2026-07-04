#ifndef __AMEE_CAMERA_H__
#define __AMEE_CAMERA_H__
#pragma once
#include "Math/AMEEMath.hpp"
#include "Components/AMEEComponent.hpp"

namespace AMEE {

class Camera : public Component {
public:
    Camera(float FovDeg = 60.0f, float Near = 0.1f, float Far = 1000.0f);

    // Rotation (stored in component, syncs to Entity)
    float GetYaw() const { return m_Yaw; }
    float GetPitch() const { return m_Pitch; }
    void SetRotation(float Yaw, float Pitch);
    void Rotate(float YawDelta, float PitchDelta);

    // Projection
    float GetFov() const { return m_FovDeg; }
    void SetFov(float FovDeg) { m_FovDeg = FovDeg; }

    // Matrices (uses Owner Entity's position)
    Mat4 GetViewMatrix() const;
    Mat4 GetProjectionMatrix(float Aspect) const;

    // Direction vectors (based on Yaw/Pitch)
    Vec3 GetForward() const;
    Vec3 GetRight() const;
    Vec3 GetUp() const;

private:
    float m_Yaw    = -90.0f;
    float m_Pitch  = 0.0f;
    float m_FovDeg;
    float m_Near;
    float m_Far;
};

} // namespace AMEE

#endif // __AMEE_CAMERA_H__