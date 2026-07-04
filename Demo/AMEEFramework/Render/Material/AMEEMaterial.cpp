#include "AMEEMaterial.hpp"
#include "../Shader/AMEEShaderProgram.hpp"
#include "../Texture/AMEETexture2D.hpp"
#include "../Texture/AMEEImage.hpp"
#include "../../Core/Asset/AMEEAssetManager.hpp"
#include "../../Core/Log/AMEELog.hpp"
#include "../../Render/AMEERHI.hpp"

namespace AMEE {

Material::Material()
    : Object(ObjectType::Resoursce)
{
    SetName("New Material");
}

void Material::SetTexture(const std::string& Name, TextureHandle Handle)
{
    m_Textures[Name] = Handle;
}

TextureHandle Material::GetTexture(const std::string& Name) const
{
    auto It = m_Textures.find(Name);
    return It != m_Textures.end() ? It->second : TextureHandle();
}

void Material::SetFloat(const std::string& Name, float Value)
{
    m_Floats[Name] = Value;
}

float Material::GetFloat(const std::string& Name, float Default) const
{
    auto It = m_Floats.find(Name);
    return It != m_Floats.end() ? It->second : Default;
}

void Material::SetVec4(const std::string& Name, const Vec4& Value)
{
    m_Vectors[Name] = Value;
}

Vec4 Material::GetVec4(const std::string& Name, const Vec4& Default) const
{
    auto It = m_Vectors.find(Name);
    return It != m_Vectors.end() ? It->second : Default;
}

void Material::Apply(RHI* rhi)
{
    auto& Assets = AssetManager::Instance();
    ShaderProgram* Shader = Assets.GetShader(m_Shader);
    if (!Shader) return;

    Shader->use();

    static std::unique_ptr<Texture2D> gs_pFallbackTex;
    auto EnsureFallback = [&]() {
        if (!gs_pFallbackTex && rhi) {
            unsigned char Magenta[4] = { 255, 0, 255, 255 };
            ImageData Img;
            Img.Width = Img.Height = 1;
            Img.Channels = 4;
            Img.Pixels.assign(Magenta, Magenta + 4);
            gs_pFallbackTex = std::make_unique<Texture2D>();
            gs_pFallbackTex->Create(rhi, Img);
        }
    };

    if (m_Textures.empty()) {
        EnsureFallback();
        if (gs_pFallbackTex) {
            gs_pFallbackTex->Bind(0);
            Shader->setInt("u_MainTex", 0);
        }
        return;
    }

    int Slot = 0;
    for (auto& KV : m_Textures) {
        Texture2D* Tex = Assets.GetTexture(KV.second);
        if (!Tex) {
            AMEE_LOG_WARN("Material", "[%s] Missing texture: %s", GetName().c_str(), KV.first.c_str());
            EnsureFallback();
            Tex = gs_pFallbackTex.get();
        }
        if (Tex) {
            Tex->Bind(Slot);
            Shader->setInt(KV.first, Slot);
            Slot++;
        }
    }

    for (auto& KV : m_Floats) {
        Shader->setFloat(KV.first, KV.second);
    }

    for (auto& KV : m_Vectors) {
        Shader->setFloat4(KV.first, KV.second.x, KV.second.y, KV.second.z, KV.second.w);
    }
}

void Material::PrintDebug() const
{
    auto& Assets = AssetManager::Instance();
    AMEE_LOG_INFO("Material", "=== [%s] (ID=%llu) ===", GetName().c_str(), GetID());

    ShaderProgram* Shader = Assets.GetShader(m_Shader);
    AMEE_LOG_INFO("Material", "  Shader  [%s] %s",
                  m_Shader.IsValid() ? "valid" : "MISSING",
                  Shader ? "OK" : "NOT FOUND");

    if (m_Textures.empty()) {
        AMEE_LOG_INFO("Material", "  Textures: (none — using fallback pink)");
    } else {
        for (auto& KV : m_Textures) {
            Texture2D* Tex = Assets.GetTexture(KV.second);
            AMEE_LOG_INFO("Material", "  Texture [%s] handle=%s %s",
                          KV.first.c_str(),
                          KV.second.IsValid() ? "valid" : "INVALID",
                          Tex ? "OK" : "MISSING → PINK");
        }
    }

    for (auto& KV : m_Floats) {
        AMEE_LOG_INFO("Material", "  Float   [%s] = %.3f", KV.first.c_str(), KV.second);
    }
    for (auto& KV : m_Vectors) {
        AMEE_LOG_INFO("Material", "  Vec4    [%s] = (%.2f, %.2f, %.2f, %.2f)",
                      KV.first.c_str(), KV.second.x, KV.second.y, KV.second.z, KV.second.w);
    }
}

} // namespace AMEE
