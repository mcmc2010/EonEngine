#include "AMEEScene.hpp"
#include "Entity/AMEEEntity.hpp"
#include "Components/AMEEComponent.hpp"
#include "Components/AMEELight.hpp"
#include "Components/AMEEMeshFilter.hpp"
#include "Components/AMEEMeshRenderer.hpp"
#include "Components/AMEEGridHelper.hpp"
#include "AMEECamera.hpp"
#include "../Render/Shader/AMEEShaderProgram.hpp"
#include "../Render/AMEEMesh.hpp"
#include "../Render/AMEERHI.hpp"
#include "../Render/Material/AMEEMaterial.hpp"
#include "../Core/Asset/AMEEAssetManager.hpp"
#include "Log/AMEELog.hpp"

namespace AMEE {

////
bool Scene::AddChild(std::unique_ptr<Node> child)
{
    // 1. 在移动前提取原始指针
    Node* rawPtr = child.get();
    Entity* entity = dynamic_cast<Entity*>(rawPtr);

    // 2. 调用父类添加（转移所有权）
    if (!Node::AddChild(std::move(child))) {
        return false;
    }

    // 3. 如果是 Entity，触发添加事件
    if (entity) {
        return OnAddedChild(entity);
    }
    return true;
}

std::unique_ptr<Node> Scene::RemoveChild(Node* child)
{
    auto* entity = dynamic_cast<Entity*>(child);
    if (entity) {
        this->OnRemovedChild(entity);
    }
    
    //
    return Node::RemoveChild(child);
}

bool Scene::OnAddedChild(Entity *entity)
{
    if(!entity) {
        return false;
    }
    
    // 1. Set scene reference
    entity->SetScene(this);

    // 2. Process all components
    for (const auto& comp : entity->GetAllComponents()) {
        if (!OnAddedComponent(entity, comp.get())) {
            return false;
        }
    }

    // 3. Recursively process child entities
    for (auto& child : entity->GetChildren()) {
        Entity* subEntity = dynamic_cast<Entity*>(child.get());
        if (subEntity) {
            if (!OnAddedChild(subEntity)) {
                return false;
            }
        }
    }
    return true;
}


void Scene::OnRemovedChild(Entity* entity)
{
    if(!entity) {
        return;
    }
    
    // 1. Process all components
    for (const auto& comp : entity->GetAllComponents()) {
        OnRemovedComponent(entity, comp.get());
    }

    // 2. Recursively process child entities
    for (auto& child : entity->GetChildren()) {
        Entity* subEntity = dynamic_cast<Entity*>(child.get());
        if (subEntity) {
            OnRemovedChild(subEntity);
        }
    }

    // 3. Clear scene reference
    entity->SetScene(nullptr);
}

bool Scene::OnAddedComponent(Entity *entity, Component *comp)
{
    // 检测主相机
    if (auto* camera = dynamic_cast<Camera*>(comp)) {
        if (entity->GetTag() == TAG_MAINCAMERA) {
            if (m_pMainCamera && m_pMainCamera != camera) {
                AMEE_LOG_WARN("Scene", "Multiple main cameras detected, replacing previous one");
                // 可选：将旧相机的 Tag 改为 "Untagged"
                m_pMainCamera->GetOwner()->SetTag(TAG_UNTAGGED);
            }
            m_pMainCamera = camera;
        }
    }
    return true;
}

void Scene::OnRemovedComponent(Entity *ent, Component *comp)
{
    // 如果移除的是主相机组件，清理指针
    if (auto* camera = dynamic_cast<Camera*>(comp)) {
        if (m_pMainCamera == camera) {
            m_pMainCamera = nullptr;
            AMEE_LOG_INFO("Scene", "Main camera component removed, pointer cleared");
        }
    }
}

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

    Mat4 Proj = pCamera->GetProjectionMatrix();
    Mat4 VP = Proj * RotView;
    Mat4 InvVP = VP.Inverse();

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

void Scene::Render(RHI* rhi, ICamera* camera)
{
    if (!m_pMainCamera) return;

    Mat4 VP = m_pMainCamera->GetProjectionMatrix() * m_pMainCamera->GetViewMatrix();

    PreRenderChildren(rhi, VP, this->GetChildren());
    RenderChildren(rhi, VP, this->GetChildren());
    PostRenderChildren(rhi, VP, this->GetChildren());
}

void Scene::PreRenderChildren(RHI* rhi, const Mat4& ViewProj, const std::vector<std::unique_ptr<Node>>& Children)
{
    for (auto& Child : Children) {
        if (!Child || !Child->IsActive()) continue;

        if (auto* Ent = dynamic_cast<Entity*>(Child.get())) {
            OnPreRender(rhi, ViewProj, Ent);
        }
        PreRenderChildren(rhi, ViewProj, Child->GetChildren());
    }
}

void Scene::PostRenderChildren(RHI* rhi, const Mat4& ViewProj, const std::vector<std::unique_ptr<Node>>& Children)
{
    for (auto& Child : Children) {
        if (!Child || !Child->IsActive()) continue;

        if (auto* Ent = dynamic_cast<Entity*>(Child.get())) {
            OnPostRender(rhi, ViewProj, Ent);
        }
        PostRenderChildren(rhi, ViewProj, Child->GetChildren());
    }
}

void Scene::RenderChildren(RHI* rhi, const Mat4& ViewProj, const std::vector<std::unique_ptr<Node>>& Children)
{
    for (auto& Child : Children) {
        if (!Child || !Child->IsActive()) continue;

        if (auto* Ent = dynamic_cast<Entity*>(Child.get())) {

            OnRender(rhi, ViewProj, Ent);

        }

        RenderChildren(rhi, ViewProj, Child->GetChildren());
    }
}

void Scene::OnPreRender(RHI* rhi, const Mat4& ViewProj, Entity* entity)
{
    // Check if entity's layer is visible to the main camera
    if (m_pMainCamera) {
        LayerMask entityLayer = entity->GetLayer();
        LayerMask cullingMask = m_pMainCamera->GetCullingMask();
        if (!cullingMask.ContainsAny(entityLayer)) {
            return;  // Skip - not in camera's culling mask
        }
    }
    
    // Draw editor/helper gizmos
    if (auto* grid = entity->GetComponent<GridHelper>()) {
        if(grid->IsVisible()) {
            grid->Draw(rhi, ViewProj);
        }
    }
}

void Scene::OnRender(RHI* rhi, const Mat4& ViewProj, Entity* entity)
{
    // Check if entity's layer is visible to the main camera
    if (m_pMainCamera) {
        LayerMask entityLayer = entity->GetLayer();
        LayerMask cullingMask = m_pMainCamera->GetCullingMask();
        if (!cullingMask.ContainsAny(entityLayer)) {
            return;  // Skip - not in camera's culling mask
        }
    }

    if (auto* Renderer = entity->GetComponent<MeshRenderer>()) {
        if (Renderer->IsVisible()) {
            Renderer->Draw(rhi, ViewProj);
        }
    }
}

void Scene::RenderDepth(RHI* rhi, ICamera* camera)
{
    if (!camera) return;
    Mat4 VP = camera->GetViewProjection();
    RenderDepthChildren(rhi, VP, this->GetChildren());
}

void Scene::RenderDepthChildren(RHI* rhi, const Mat4& LightVP, const std::vector<std::unique_ptr<Node>>& Children)
{
    for (auto& Child : Children) {
        if (!Child || !Child->IsActive()) continue;

        if (auto* Ent = dynamic_cast<Entity*>(Child.get())) {
            if (auto* Renderer = Ent->GetComponent<MeshRenderer>()) {
                if (Renderer->IsVisible() && Renderer->CastsShadow()) {
                    Renderer->Draw(rhi, LightVP);
                }
            }
        }

        RenderDepthChildren(rhi, LightVP, Child->GetChildren());
    }
}

} // namespace AMEE
