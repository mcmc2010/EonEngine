#include "AMEERenderPipeline.hpp"
#include "../Core/AMEEShadowCamera.hpp"
#include "../Core/AMEEScene.hpp"
#include "../Core/AMEECamera.hpp"
#include "../Core/Entity/AMEEEntity.hpp"
#include "../Core/Components/AMEELight.hpp"
#include "AMEERHI.hpp"
#include "../Core/Log/AMEELog.hpp"

namespace AMEE {

RenderPipeline::RenderPipeline(RHI* rhi)
    : m_pRHI(rhi)
{
}

RenderPipeline::~RenderPipeline()
{
    for (auto& Shadow : m_ShadowMaps) {
        if (m_pRHI) {
            m_pRHI->destroyFramebuffer(Shadow.FBO);
        }
        delete Shadow.ShadowCamera;
    }
    m_ShadowMaps.clear();
}

void RenderPipeline::Render(Scene* scene)
{
    if (!m_pRHI || !scene) return;

    Camera* camera = scene->GetMainCamera();
    if (!camera) {
        AMEE_LOG_WARN("RenderPipeline", "No main camera found");
        return;
    }

    // Collect lights once
    scene->CollectLights();

    // 1. Shadow Pass
    RenderShadowMaps(m_pRHI, scene);

    // 2. Main Pass
    m_pRHI->bindDefaultFramebuffer();

    m_pRHI->setClearColor(scene->GetAmbientColor().x,
                          scene->GetAmbientColor().y,
                          scene->GetAmbientColor().z, 1.0f);
    m_pRHI->clear();

    // Draw skybox
    scene->DrawSkybox(m_pRHI, camera);

    // Render scene
    RenderScene(m_pRHI, scene, camera);
}

void RenderPipeline::RenderScene(RHI* rhi, Scene* scene, Camera* camera)
{
    scene->Render(rhi, camera);
}

void RenderPipeline::RenderShadowMaps(RHI* rhi, Scene* scene)
{
    // Allocate FBOs if needed
    while (m_ShadowMaps.size() < scene->GetAllLights().size()) {
        DirectionalShadowData Data;
        Data.FBO = rhi->createFramebuffer(m_ShadowResolution, m_ShadowResolution);
        Data.ShadowCamera = new ShadowCamera();
        m_ShadowMaps.push_back(Data);
    }

    int LightIdx = 0;
    for (auto* Light : scene->GetAllLights()) {
        if (Light->m_Type != Light::Type::Directional) continue;
        if (LightIdx >= (int)m_ShadowMaps.size()) break;

        auto& ShadowData = m_ShadowMaps[LightIdx];
        Entity* Owner = Light->GetOwner();
        if (!Owner) { LightIdx++; continue; }

        // Configure shadow camera from light direction
        Vec3 LightDir = Owner->GetForward();
        Camera* MainCam = scene->GetMainCamera();
        Vec3 SceneCenter = MainCam ? MainCam->GetOwner()->GetPosition() : Vec3(0);
        
        ShadowData.ShadowCamera->ConfigureFromLight(LightDir, SceneCenter, 15.0f);

        // Render depth from light's view
        rhi->bindFramebuffer(ShadowData.FBO);
        rhi->setViewport({0.0f, 0.0f, (float)m_ShadowResolution, (float)m_ShadowResolution});
        rhi->setClearColor(1.0f, 1.0f, 1.0f, 1.0f);
        rhi->clear();

        // Render depth from light's view using ShadowCamera
        scene->RenderDepth(rhi, ShadowData.ShadowCamera);

        LightIdx++;
    }

    // Return to default framebuffer
    rhi->bindDefaultFramebuffer();
}

} // namespace AMEE