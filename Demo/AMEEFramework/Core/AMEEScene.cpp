#include "AMEEScene.hpp"
#include "Entity/AMEEEntity.hpp"
#include "Components/AMEELight.hpp"
#include "../Render/Shader/AMEEShaderProgram.hpp"
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

void Scene::CollectLights()
{
    m_Lights.clear();
    CollectLightsRecursive(GetChildren(), m_Lights);

    // Pick first Directional light for sun data
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
    // Collect lights before updating entities
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
