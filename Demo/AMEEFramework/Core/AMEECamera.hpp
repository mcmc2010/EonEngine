#ifndef __AMEE_CAMERA_H__
#define __AMEE_CAMERA_H__
#pragma once
#include "Math/AMEEMath.hpp"
#include "Components/AMEEComponent.hpp"

namespace AMEE {

enum class CameraType : uint8_t {
    Base = 0,
    Overlay = 1,
};

enum class CameraUsage : uint8_t {
    Main,
    Custom,
};

enum class CameraProjection : uint8_t {
    Perspective,
    Orthographic
};

class IPlatformContext;

class Camera : public Component {
public:
    Camera(float FovDeg = 60.0f, float Near = 0.1f, float Far = 1000.0f);

    // Camera type
    CameraType GetCameraType() const { return m_CameraType; }
    void SetCameraType(CameraType Type) { m_CameraType = Type; }

    CameraUsage GetCameraUsage() const { return m_CameraUsage; }
    void SetCameraUsage(CameraUsage Usage) { m_CameraUsage = Usage; }

    CameraProjection GetProjectionType() const { return m_ProjectionType; }
    void SetProjectionType(CameraProjection Type) { m_ProjectionType = Type; }

    LayerMask GetCullingMask() const { return m_CullingMask; }
    void SetCullingMask(LayerMask mask) { m_CullingMask = mask; }
    
    // Rotation
    float GetYaw() const { return m_Yaw; }
    float GetPitch() const { return m_Pitch; }
    void SetRotation(float Yaw, float Pitch);
    void Rotate(float YawDelta, float PitchDelta);

    // Projection
    float GetFov() const { return m_FovDeg; }
    void SetFov(float FovDeg) { m_FovDeg = FovDeg; }

    // Aspect ratio
    void SetAspect(float Aspect) { m_Aspect = Aspect; }
    float GetAspect() const { return m_Aspect; }
    void UpdateAspect(IPlatformContext* ctx);

    // Matrices (uses Owner Entity's position)
    Mat4 GetViewMatrix() const;
    Mat4 GetProjectionMatrix() const;

    // Direction vectors (based on Yaw/Pitch)
    Vec3 GetForward() const;
    Vec3 GetRight() const;
    Vec3 GetUp() const;

private:
    CameraType m_CameraType = CameraType::Base;
    CameraUsage m_CameraUsage = CameraUsage::Main;
    CameraProjection m_ProjectionType = CameraProjection::Perspective;

    //
    LayerMask m_CullingMask = LayerMask::Everything(); // 默认全看
    
    //
    float m_Yaw    = -90.0f;
    float m_Pitch  = 0.0f;
    float m_FovDeg;
    float m_Near;
    float m_Far;
    float m_Aspect = 16.0f / 9.0f;
};

} // namespace AMEE

#endif // __AMEE_CAMERA_H__
