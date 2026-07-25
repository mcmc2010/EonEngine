#ifndef __AMEE_SHADOWCAMERA_H__
#define __AMEE_SHADOWCAMERA_H__
#pragma once
#include "AMEEICamera.hpp"

namespace AMEE {

class ShadowCamera : public ICamera {
public:
    ShadowCamera() = default;
    ~ShadowCamera() override = default;

    // Position and direction
    void SetPosition(const Vec3& Pos) { m_Position = Pos; }
    void SetDirection(const Vec3& Dir) { m_Direction = Dir.Normalized(); }
    void SetUp(const Vec3& Up) { m_Up = Up; }

    // Orthographic projection parameters
    void SetOrthoSize(float Left, float Right, float Bottom, float Top) {
        m_Left = Left; m_Right = Right;
        m_Bottom = Bottom; m_Top = Top;
    }
    void SetNearFar(float Near, float Far) { m_Near = Near; m_Far = Far; }

    // Auto-configure from light and scene bounds
    void ConfigureFromLight(const Vec3& LightDir, const Vec3& SceneCenter, float SceneRadius);

    // ICamera interface
    Mat4 GetViewMatrix() const override;
    Mat4 GetProjectionMatrix() const override;
    Vec3 GetPosition() const override { return m_Position; }
    Vec3 GetForward() const override { return m_Direction; }

private:
    Vec3 m_Position = {0, 0, 0};
    Vec3 m_Direction = {0, -1, 0};
    Vec3 m_Up = {0, 1, 0};

    // Orthographic projection
    float m_Left = -15.0f;
    float m_Right = 15.0f;
    float m_Bottom = -15.0f;
    float m_Top = 15.0f;
    float m_Near = 1.0f;
    float m_Far = 50.0f;
};

} // namespace AMEE

#endif // __AMEE_SHADOWCAMERA_H__