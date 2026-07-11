#ifndef __AMEE_LIGHT_H__
#define __AMEE_LIGHT_H__
#pragma once
#include "../AMEEObject.hpp"
#include "../Math/AMEEMath.hpp"

namespace AMEE {

class Light : public Component {
public:
    enum class Type : uint8_t {
        Directional = 0,
        Point,
        Spot,
    };

    Light() = default;

    Type m_Type = Type::Directional;
    Vec4 m_Color = {1.0f, 1.0f, 1.0f, 1.0f};   // RGB + Intensity
};

} // namespace AMEE

#endif // __AMEE_LIGHT_H__
