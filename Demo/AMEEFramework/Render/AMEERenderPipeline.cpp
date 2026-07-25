#include "AMEERenderPipeline.hpp"
#include "../Core/AMEEScene.hpp"
#include "../Core/AMEECamera.hpp"
#include "AMEERHI.hpp"
#include "../Core/Log/AMEELog.hpp"

namespace AMEE {

RenderPipeline::RenderPipeline(RHI* rhi)
    : m_pRHI(rhi)
{
}

void RenderPipeline::Render(Scene* scene)
{
    if (!m_pRHI || !scene) return;

    Camera* camera = scene->GetMainCamera();
    if (!camera) {
        AMEE_LOG_WARN("RenderPipeline", "No main camera found");
        return;
    }

    // Update camera aspect ratio
    // (Assuming GLContext is available through RHI or scene)

    // Calculate view-projection matrix
    Mat4 view = camera->GetViewMatrix();
    Mat4 proj = camera->GetProjectionMatrix();
    Mat4 viewProj = proj * view;

    // Clear
    m_pRHI->setClearColor(scene->GetAmbientColor().x,
                          scene->GetAmbientColor().y,
                          scene->GetAmbientColor().z, 1.0f);
    m_pRHI->clear();

    // Collect lights
    scene->CollectLights();

    // Render skybox (before scene, uses depth trick)
    scene->DrawSkybox(m_pRHI, camera);

    // Render scene with culling mask
    RenderScene(m_pRHI, scene, camera, viewProj);
}

void RenderPipeline::RenderScene(RHI* rhi, Scene* scene, Camera* camera, const Mat4& viewProj)
{
    // Scene::Render handles culling mask internally
    scene->Render(rhi);
}

} // namespace AMEE