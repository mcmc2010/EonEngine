#ifndef __AMEE_ICAMERA_H__
#define __AMEE_ICAMERA_H__
#pragma once
#include "Math/AMEEMath.hpp"

namespace AMEE {

// Camera interface for unified camera management
class ICamera {
public:
    virtual ~ICamera() = default;

    virtual Mat4 GetViewMatrix() const = 0;
    virtual Mat4 GetProjectionMatrix() const = 0;
    virtual Mat4 GetViewProjection() const {
        return GetProjectionMatrix() * GetViewMatrix();
    }

    virtual Vec3 GetPosition() const = 0;
    virtual Vec3 GetForward() const = 0;
};

} // namespace AMEE

#endif // __AMEE_ICAMERA_H__