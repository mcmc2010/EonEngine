#include "AMEESkyboxMaterial.hpp"
#include "../Texture/AMEEImage.hpp"
#include "../Shader/AMEEShaderProgram.hpp"
#include "../Texture/AMEETexture2D.hpp"
#include "../../Core/Asset/AMEEAssetManager.hpp"
#include "../../Core/Asset/AMEEFileSystem.hpp"
#include "../../Core/Log/AMEELog.hpp"
#include "../../Render/AMEERHI.hpp"

namespace AMEE {

static const char* g_VsCubemap = R"(
#version 410 core
layout(location = 0) in vec3 aPos;
uniform mat4 uVP;
out vec3 vTexCoord;
void main() {
    vec4 pos = uVP * vec4(aPos, 1.0);
    gl_Position = pos.xyww;
    vTexCoord = aPos;
}
)";

static const char* g_FsCubemap = R"(
#version 410 core
in vec3 vTexCoord;
uniform samplerCube u_Cubemap;
out vec4 fragColor;
void main() {
    fragColor = texture(u_Cubemap, vTexCoord);
}
)";

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

    // Create shader
    auto& Assets = AssetManager::GetSingleton();
    auto Shader = rhi->CreateShaderProgram();
    Shader->compileFromSource(ShaderType::Vertex, g_VsCubemap, [](const ShaderCompileError& E) {
        AMEE_LOG_ERROR("Skybox", "VS error: %s", E.message.c_str());
    });
    Shader->compileFromSource(ShaderType::Fragment, g_FsCubemap, [](const ShaderCompileError& E) {
        AMEE_LOG_ERROR("Skybox", "FS error: %s", E.message.c_str());
    });
    if (!Shader->link([](const std::string& E) {
        AMEE_LOG_ERROR("Skybox", "Link error: %s", E.c_str());
    })) {
        AMEE_LOG_ERROR("SkyboxMaterial", "Shader link failed");
        return false;
    }
    SetShader(Assets.RegisterShader(std::move(Shader), "_SkyboxShader"));

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