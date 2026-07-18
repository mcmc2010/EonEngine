#include "AMEEStandardMaterial.hpp"
#include "../../Core/Asset/AMEEAssetManager.hpp"
#include "../../Core/AMEEBuiltIDs.hpp"

namespace AMEE {

StandardMaterial::StandardMaterial()
{
    SetName("Standard");
    SetAlbedoColor({1, 1, 1, 1});
    SetMetallic(0.0f);
    SetSmoothness(0.5f);
}

void StandardMaterial::SetAlbedoMap(TextureHandle Handle)
{
    SetTexture(MainTex, Handle);
}

void StandardMaterial::SetAlbedoColor(const Vec4& Color)
{
    SetVec4(ColorProp, Color);
}

void StandardMaterial::SetNormalMap(TextureHandle Handle)
{
    SetTexture(NormalMap, Handle);
}

void StandardMaterial::SetMetallic(float Value)
{
    SetFloat(Metallic, Value);
}

void StandardMaterial::SetSmoothness(float Value)
{
    SetFloat(Smoothness, Value);
}

void StandardMaterial::SetSpecularColor(const Vec4& Color)
{
    SetVec4("u_SpecColor", Color);
}

void StandardMaterial::SetSpecularExponent(float Exp)
{
    SetFloat("u_SpecExp", Exp);
}

void StandardMaterial::Apply(RHI* rhi)
{
    auto& Assets = AssetManager::GetSingleton();

    // Fallback: if no normal map set, use built-in flat normal
    TextureHandle NormalMapTex = GetTexture(NormalMap);
    if (!NormalMapTex.IsValid()) {
        NormalMapTex = Assets.GetBuiltinTexture(BuiltID::Texture_Normal);
        SetTexture(NormalMap, NormalMapTex);
    }

    Material::Apply(rhi);
}

} // namespace AMEE