#include "AMEEScene.hpp"
#include "Entity/AMEEEntity.hpp"
#include "Components/AMEELight.hpp"
#include "Components/AMEEMeshFilter.hpp"
#include "../Render/Shader/AMEEShaderProgram.hpp"
#include "../Render/AMEEMesh.hpp"
#include "../Render/AMEERHI.hpp"
#include "../Render/Material/AMEEMaterial.hpp"
#include "../Render/Material/AMEESkyboxMaterial.hpp"
#include "../Core/Meshes/AMEEPrimitiveMesh.hpp"
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

bool Scene::LoadSkybox(RHI* rhi, const std::string& Dir)
{
    std::string Base = Dir;
    if (!Base.empty() && Base.back() != '/') Base += '/';

    auto Mat = std::make_unique<SkyboxMaterial>();
    if (!Mat->LoadFaces(rhi,
            Base + "px.png", Base + "nx.png",
            Base + "py.png", Base + "ny.png",
            Base + "pz.png", Base + "nz.png"))
    {
        AMEE_LOG_ERROR("Scene", "Failed to load cubemap from %s", Dir.c_str());
        return false;
    }

    auto& Assets = AssetManager::Instance();
    m_Skybox = Assets.RegisterMaterial(std::move(Mat));

    auto SkyMesh = std::unique_ptr<Mesh>(PrimitiveMesh::CreateCube(rhi, 2.0f));
    m_SkyboxMesh = Assets.RegisterMesh(std::move(SkyMesh), "_SkyboxCube");

    AMEE_LOG_INFO("Scene", "Skybox loaded from %s", Dir.c_str());
    return true;
}

void Scene::DrawSkybox(RHI* rhi, const Mat4& View, const Mat4& Proj)
{
    if (!m_Skybox.IsValid() || !m_SkyboxMesh.IsValid()) return;

    auto& Assets = AssetManager::Instance();
    Material* Mat = Assets.GetMaterial(m_Skybox);
    Mesh* SkyMesh = Assets.GetMesh(m_SkyboxMesh);
    if (!Mat || !SkyMesh) return;

    rhi->setDepthMask(false);

    Mat4 SkyVP = Proj * View;
    SkyVP.at(3, 0) = SkyVP.at(3, 1) = SkyVP.at(3, 2) = 0;

    Mat->Apply(rhi);
    Assets.GetShader(Mat->GetShader())->setMat4("uVP", SkyVP.Data());
    SkyMesh->Draw();

    rhi->setDepthMask(true);
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
