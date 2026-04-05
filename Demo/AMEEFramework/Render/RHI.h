#pragma once
#include "RHITypes.h"

class RHI {
public:
    virtual ~RHI() = default;

    virtual void setClearColor(float r, float g, float b, float a) = 0;
    virtual void clear() = 0;
    virtual void setViewport(const RHIViewport& vp) = 0;
    virtual void present() = 0;
};
