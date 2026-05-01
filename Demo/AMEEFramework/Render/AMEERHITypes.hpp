#ifndef __AMEE_RHITYPES_H__
#define __AMEE_RHITYPES_H__
#pragma once
#include <cstdint>

namespace AMEE {

struct RHIViewport {
    float x = 0.0f;
    float y = 0.0f;
    float width = 0.0f;
    float height = 0.0f;
};

enum class RHIFormat : uint8_t {
    R8,
    RG8,
    RGB8,
    RGBA8,
};

enum class RHIFilter : uint8_t {
    Nearest,
    Linear,
};

enum class RHIWrap : uint8_t {
    Repeat,
    ClampToEdge,
    MirroredRepeat,
};

}

#endif // __AMEE_RHITYPES_H__