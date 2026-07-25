#ifndef __AMEE_RENDERPIPELINE_H__
#define __AMEE_RENDERPIPELINE_H__
#pragma once
#include "../Core/Math/AMEEMath.hpp"
#include "../Render/AMEERHITypes.hpp"
#include <vector>

namespace AMEE {

class RHI;
class Scene;
class Camera;
class ShadowCamera;

struct DirectionalShadowData {
    RHIFramebuffer FBO;
    ShadowCamera* ShadowCamera = nullptr;
};

class RenderPipeline {
public:
    RenderPipeline(RHI* rhi);
    ~RenderPipeline();

    void Render(Scene* scene);

    // Shadow configuration
    void SetShadowResolution(int Res) { m_ShadowResolution = Res; }
    int GetShadowResolution() const { return m_ShadowResolution; }

private:
    void RenderScene(RHI* rhi, Scene* scene, Camera* camera);
    void RenderShadowMaps(RHI* rhi, Scene* scene);

    RHI* m_pRHI;
    int m_ShadowResolution = 2048;
    std::vector<DirectionalShadowData> m_ShadowMaps;
};

} // namespace AMEE

#endif // __AMEE_RENDERPIPELINE_H__
