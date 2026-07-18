#ifndef __AMEE_STANDARDMATERIAL_H__
#define __AMEE_STANDARDMATERIAL_H__
#pragma once
#include "AMEEMaterial.hpp"

namespace AMEE {

class StandardMaterial : public Material {
public:
    StandardMaterial();

    void SetAlbedoMap(TextureHandle Handle);
    void SetAlbedoColor(const Vec4& Color);
    void SetNormalMap(TextureHandle Handle);
    void SetMetallic(float Value);
    void SetSmoothness(float Value);
    void SetSpecularColor(const Vec4& Color);
    void SetSpecularExponent(float Exp);

    void Apply(RHI* rhi) override;

private:
    static constexpr const char* MainTex    = "u_MainTex";  // 贴图
    static constexpr const char* ColorProp  = "u_Color";
    static constexpr const char* NormalMap  = "u_NormalMap";// 法线
    static constexpr const char* Metallic   = "u_Metallic";
    static constexpr const char* Smoothness = "u_Smoothness";
};

} // namespace AMEE

#endif // __AMEE_STANDARDMATERIAL_H__
