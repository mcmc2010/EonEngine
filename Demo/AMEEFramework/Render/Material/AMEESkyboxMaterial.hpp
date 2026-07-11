#ifndef __AMEE_SKYBOXMATERIAL_H__
#define __AMEE_SKYBOXMATERIAL_H__
#pragma once
#include "AMEEMaterial.hpp"
#include <string>

namespace AMEE {

class SkyboxMaterial : public Material {
public:
    SkyboxMaterial();

    bool LoadFaces(RHI* rhi,
                   const std::string& Right,  const std::string& Left,
                   const std::string& Top,    const std::string& Bottom,
                   const std::string& Front,  const std::string& Back);

    void Apply(RHI* rhi) override;

private:
    uint32_t m_CubemapID = 0;
    RHI* m_pRHI = nullptr;
};

} // namespace AMEE

#endif // __AMEE_SKYBOXMATERIAL_H__
