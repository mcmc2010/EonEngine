#include "AMEEScene.hpp"
#include "AMEECamera.hpp"
#include "Entity/AMEEEntity.hpp"
#include "Components/AMEELight.hpp"
#include "Components/AMEEMeshFilter.hpp"
#include "../Render/Shader/AMEEShaderProgram.hpp"
#include "../Render/AMEEMesh.hpp"
#include "../Render/AMEERHI.hpp"
#include "../Render/Material/AMEEMaterial.hpp"
#include "../Core/Asset/AMEEAssetManager.hpp"
#include "Log/AMEELog.hpp"

namespace AMEE {

static void CollectLightsRecursive(const std::vector<std::unique_ptr<Node>>& Children,
                                    std::vector<Light*>& OutLights)
{
    for (auto& Child : Children) {
        if (auto* Ent = dynamic_cast<Entity*>(Child.get())) {
            auto Lights = Ent->GetComponents<Light>();
            for (auto* L : Lights) {
                OutLights.push_back(L);
            }
        }
        CollectLightsRecursive(Child->GetChildren(), OutLights);
    }
}

bool Scene::LoadSkybox(RHI* rhi, const std::string& Path)
{
    auto& Assets = AssetManager::GetSingleton();

    // Detect if path is an image file (cross format) or directory (6 separate images)
    bool IsCrossImage = (Path.find(".png") != std::string::npos ||
                         Path.find(".jpg") != std::string::npos ||
                         Path.find(".jpeg") != std::string::npos ||
                         Path.find(".bmp") != std::string::npos ||
                         Path.find(".tga") != std::string::npos);

    if (IsCrossImage) {
        m_SkyboxCubemap = Assets.LoadCubemapCross(rhi, Path);
    } else {
        m_SkyboxCubemap = Assets.LoadCubemap(rhi, Path);
    }

    if (!m_SkyboxCubemap.IsValid()) {
        AMEE_LOG_ERROR("Scene", "Failed to load skybox from %s", Path.c_str());
        return false;
    }

    // Use built-in skybox material
    m_Skybox = Assets.GetBuiltinMaterial(BuiltID::Material_Skybox);

    AMEE_LOG_INFO("Scene", "Skybox loaded from %s", Path.c_str());
    return true;
}

void Scene::DrawSkybox(RHI* rhi, Camera* pCamera)
{
    if (!m_Skybox.IsValid() || !m_SkyboxCubemap.IsValid() || !pCamera) return;

    auto& Assets = AssetManager::GetSingleton();
    Material* Mat = Assets.GetMaterial(m_Skybox);
    ShaderProgram* Shader = Assets.GetShader(Mat->GetShader());
    if (!Mat || !Shader) return;

    // Rotation-only view matrix (remove translation)
    Mat4 View = pCamera->GetViewMatrix();
    Mat4 RotView = View;
    RotView.at(3, 0) = 0;
    RotView.at(3, 1) = 0;
    RotView.at(3, 2) = 0;

    //Mat4 Proj = pCamera->GetProjectionMatrix();
    Mat4 Proj = Mat4::Perspective(90.0f, pCamera->GetAspect(), 0.1f, 1000.0f);
    Mat4 VP = Proj * RotView;
    Mat4 InvVP = VP.InverseTransform();

    // Skybox render state
    rhi->setDepthMask(false);       // Disable depth writing
    rhi->setDepthFunc(true);        // Set depth func to LEQUAL (z=1.0 passes)

    Shader->use();
    uint32_t cubemapGLID = Assets.GetCubemap(m_SkyboxCubemap);
    rhi->bindCubemap(cubemapGLID, 0);
    Shader->setInt("u_Cubemap", 0);
    Shader->setMat4("u_InverseVP", InvVP.Data());

    static uint32_t dummyVAO = 0;
    if (dummyVAO == 0) dummyVAO = rhi->createVertexArray();
    rhi->bindVertexArray(dummyVAO);
    rhi->drawArrays(RHIPrimitive::Triangles, 3, 0);
    rhi->bindVertexArray(0);

    // Restore render state
    rhi->setDepthMask(true);        // Re-enable depth writing
    rhi->setDepthFunc(false);       // Restore depth func to LESS
}

void Scene::CollectLights()
{
    m_Lights.clear();
    CollectLightsRecursive(GetChildren(), m_Lights);

    m_SunData = {};
    for (auto* L : m_Lights) {
        if (L->m_Type == Light::Type::Directional) {
            Entity* Owner = L->GetOwner();
            if (Owner) {
                m_SunData.Direction = Owner->GetForward();
                m_SunData.Color = L->m_Color;
                break;
            }
        }
    }
}

void Scene::ApplyLighting(ShaderProgram* Shader) const
{
    if (!Shader) return;

    Shader->setFloat4("u_AmbientColor", m_AmbientColor.x, m_AmbientColor.y,
                      m_AmbientColor.z, m_AmbientColor.w);

    if (m_SunData.Color.w > 0.0f) {
        Shader->setFloat3("u_LightDir", m_SunData.Direction.x,
                          m_SunData.Direction.y, m_SunData.Direction.z);
        Shader->setFloat4("u_LightColor", m_SunData.Color.x, m_SunData.Color.y,
                          m_SunData.Color.z, m_SunData.Color.w);
    } else {
        Shader->setFloat3("u_LightDir", 0, 0, 0);
        Shader->setFloat4("u_LightColor", 0, 0, 0, 0);
    }
}

void Scene::Update(float DeltaTime)
{
    CollectLights();

    for (auto& Child : GetChildren()) {
        if (Child && Child->IsActive()) {
            if (Entity* Ent = dynamic_cast<Entity*>(Child.get())) {
                Ent->Update(DeltaTime);
            }
        }
    }
}

} // namespace AMEE
