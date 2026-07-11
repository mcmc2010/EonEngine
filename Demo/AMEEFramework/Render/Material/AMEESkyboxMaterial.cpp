#include "AMEESkyboxMaterial.hpp"
#include "../Texture/AMEEImage.hpp"
#include "../Shader/AMEEShaderProgram.hpp"
#include "../Texture/AMEETexture2D.hpp"
#include "../../Core/Asset/AMEEAssetManager.hpp"
#include "../../Core/Log/AMEELog.hpp"
#include "../../Render/AMEERHI.hpp"
#define GL_SILENCE_DEPRECATION
#include <OpenGL/gl3.h>
#include "../../Platform/macOS/GL/AMEEGLCheck.hpp"

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
    GLenum Targets[6] = {
        GL_TEXTURE_CUBE_MAP_POSITIVE_X,
        GL_TEXTURE_CUBE_MAP_NEGATIVE_X,
        GL_TEXTURE_CUBE_MAP_POSITIVE_Y,
        GL_TEXTURE_CUBE_MAP_NEGATIVE_Y,
        GL_TEXTURE_CUBE_MAP_POSITIVE_Z,
        GL_TEXTURE_CUBE_MAP_NEGATIVE_Z,
    };

    GL_CHECK(glGenTextures(1, &m_CubemapID));
    GL_CHECK(glBindTexture(GL_TEXTURE_CUBE_MAP, m_CubemapID));

    int FaceW = 0, FaceH = 0;
    for (int I = 0; I < 6; I++) {
        ImageData Img = LoadImage(Paths[I]);
        if (Img.Pixels.empty()) {
            AMEE_LOG_ERROR("SkyboxMaterial", "Failed to load: %s", Paths[I].c_str());
            GL_CHECK(glDeleteTextures(1, &m_CubemapID));
            m_CubemapID = 0;
            return false;
        }
        FaceW = Img.Width; FaceH = Img.Height;
        GL_CHECK(glTexImage2D(Targets[I], 0, GL_RGBA8, Img.Width, Img.Height, 0,
                               GL_RGBA, GL_UNSIGNED_BYTE, Img.Pixels.data()));
    }

    GL_CHECK(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
    GL_CHECK(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
    GL_CHECK(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
    GL_CHECK(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));
    GL_CHECK(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE));

    // Create shader
    auto& Assets = AssetManager::Instance();
    auto Shader = rhi->CreateShaderProgram();
    Shader->compileFromSource(ShaderType::Vertex, g_VsCubemap);
    Shader->compileFromSource(ShaderType::Fragment, g_FsCubemap);
    Shader->link();
    SetShader(Assets.RegisterShader(std::move(Shader), "_SkyboxShader"));

    AMEE_LOG_INFO("SkyboxMaterial", "Cubemap loaded (%dx%d)", FaceW, FaceH);
    return true;
}

void SkyboxMaterial::Apply(RHI* rhi)
{
    auto& Assets = AssetManager::Instance();
    ShaderProgram* Shader = Assets.GetShader(m_Shader);
    if (!Shader || !m_CubemapID) return;

    Shader->use();

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, m_CubemapID);
    Shader->setInt("u_Cubemap", 0);
}

} // namespace AMEE
