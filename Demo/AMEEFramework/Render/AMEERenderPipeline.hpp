#ifndef __AMEE_RENDERPIPELINE_H__
#define __AMEE_RENDERPIPELINE_H__
#pragma once
#include "../Core/Math/AMEEMath.hpp"

namespace AMEE {

class RHI;
class Scene;
class Camera;

class RenderPipeline {
public:
    RenderPipeline(RHI* rhi);
    ~RenderPipeline() = default;

    void Render(Scene* scene);

private:
    void RenderScene(RHI* rhi, Scene* scene, Camera* camera, const Mat4& viewProj);

    RHI* m_pRHI;
};

} // namespace AMEE

#endif // __AMEE_RENDERPIPELINE_H__