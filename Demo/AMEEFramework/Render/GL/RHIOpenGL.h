#pragma once
#include "../RHI.h"

class RHIOpenGL : public RHI {
public:
    RHIOpenGL() = default;
    ~RHIOpenGL() override = default;

    void setClearColor(float r, float g, float b, float a) override;
    void clear() override;
    void setViewport(const RHIViewport& vp) override;
    void present() override;
};
