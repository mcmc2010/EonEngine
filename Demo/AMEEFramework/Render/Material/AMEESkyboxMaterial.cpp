#include "AMEESkyboxMaterial.hpp"
#include "../Texture/AMEEImage.hpp"
#include "../Shader/AMEEShaderProgram.hpp"
#include "../Texture/AMEETexture2D.hpp"
#include "../../Core/Asset/AMEEAssetManager.hpp"
#include "../../Core/Asset/AMEEFileSystem.hpp"
#include "../../Core/Log/AMEELog.hpp"
#include "../../Core/AMEEBuiltIDs.hpp"
#include "../../Render/AMEERHI.hpp"

namespace AMEE {

SkyboxMaterial::SkyboxMaterial()
{
    SetName("Skybox");
}

bool SkyboxMaterial::LoadFaces(RHI* rhi,
                                const std::string& Right,  const std::string& Left,
                                const std::string& Top,    const std::string& Bottom,
                                const std::string& Front,  const std::string& Back)
{
    m_pRHI = rhi;

    const std::string Paths[6] = { Right, Left, Top, Bottom, Front, Back };
    const unsigned char* FaceData[6] = {};
    int FaceW = 0, FaceH = 0;

    // Load all 6 faces
    for (int I = 0; I < 6; I++) {
        std::string ResolvedPath = FileSystem::GetSingleton().ResolvePath(Paths[I]);
        ImageData Img = LoadImage(ResolvedPath.empty() ? Paths[I] : ResolvedPath);
        if (Img.Pixels.empty()) {
            AMEE_LOG_ERROR("SkyboxMaterial", "Failed to load: %s", Paths[I].c_str());
            return false;
        }
        FaceW = Img.Width;
        FaceH = Img.Height;
        FaceData[I] = Img.Pixels.data();
    }

    // Create cubemap via RHI
    m_CubemapID = rhi->createCubemap(FaceData, FaceW, FaceH, RHIFormat::RGBA8, RHIFormat::RGBA8);
    if (m_CubemapID == 0) {
        AMEE_LOG_ERROR("SkyboxMaterial", "Failed to create cubemap");
        return false;
    }

    // Use built-in skybox shader
    auto& Assets = AssetManager::GetSingleton();
    SetShader(Assets.GetBuiltinShader(BuiltID::Shader_Skybox));

    AMEE_LOG_INFO("SkyboxMaterial", "Cubemap loaded (%dx%d)", FaceW, FaceH);
    return true;
}

void SkyboxMaterial::Apply(RHI* rhi)
{
    auto& Assets = AssetManager::GetSingleton();
    ShaderProgram* Shader = Assets.GetShader(m_Shader);
    if (!Shader || !m_CubemapID) return;

    Shader->use();
    rhi->bindCubemap(m_CubemapID, 0);
    Shader->setInt("u_Cubemap", 0);
}

void SkyboxMaterial::Destroy()
{
    if (m_pRHI && m_CubemapID) {
        m_pRHI->destroyCubemap(m_CubemapID);
        m_CubemapID = 0;
    }
}

} // namespace AMEE