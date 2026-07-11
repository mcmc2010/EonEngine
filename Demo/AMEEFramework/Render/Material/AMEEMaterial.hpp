#ifndef __AMEE_MATERIAL_H__
#define __AMEE_MATERIAL_H__
#pragma once
#include "../../Core/AMEEObject.hpp"
#include "../../Core/Asset/AMEEAssetHandle.hpp"
#include "../../Core/Math/AMEEMath.hpp"
#include <string>
#include <unordered_map>

namespace AMEE {

class RHI;

class Material : public Object {
public:
    Material();
    virtual ~Material() = default;

    void SetShader(ShaderHandle Handle) { m_Shader = Handle; }
    ShaderHandle GetShader() const { return m_Shader; }

    void SetTexture(const std::string& Name, TextureHandle Handle);
    TextureHandle GetTexture(const std::string& Name) const;

    void SetFloat(const std::string& Name, float Value);
    float GetFloat(const std::string& Name, float Default = 0.0f) const;

    void SetVec4(const std::string& Name, const Vec4& Value);
    Vec4 GetVec4(const std::string& Name, const Vec4& Default = {1,1,1,1}) const;

    virtual void Apply(RHI* rhi);
    void PrintDebug() const;

protected:
    ShaderHandle m_Shader;

private:
    std::unordered_map<std::string, TextureHandle> m_Textures;
    std::unordered_map<std::string, float> m_Floats;
    std::unordered_map<std::string, Vec4> m_Vectors;
};

} // namespace AMEE

#endif // __AMEE_MATERIAL_H__
