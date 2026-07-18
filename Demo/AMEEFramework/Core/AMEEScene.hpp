#ifndef __AMEE_SCENE_H__
#define __AMEE_SCENE_H__
#pragma once
#include "AMEENode.hpp"
#include "AMEEObject.hpp"
#include "Asset/AMEEAssetHandle.hpp"
#include "Math/AMEEMath.hpp"
#include <vector>

namespace AMEE {

class Entity;
class Light;
class ShaderProgram;
class RHI;
class Camera;

struct DirectionalLightData {
    Vec3 Direction;
    Vec4 Color;       // RGB + Intensity
};

class Scene : public Node, public Object {
public:
    Scene() = default;
    virtual ~Scene() = default;

    void Update(float DeltaTime);
    void Render(RHI* rhi, const Mat4& ViewProj);

    // Skybox & Environment
    void SetSkybox(MaterialHandle Handle) { m_Skybox = Handle; }
    MaterialHandle GetSkybox() const { return m_Skybox; }
    bool LoadSkybox(RHI* rhi, const std::string& Path);
    void DrawSkybox(RHI* rhi, Camera* pCamera);
    void SetAmbientColor(const Vec4& Color) { m_AmbientColor = Color; }
    Vec4 GetAmbientColor() const { return m_AmbientColor; }

    // Lighting
    void CollectLights();
    void ApplyLighting(ShaderProgram* Shader) const;
    std::vector<Light*> GetAllLights() const { return m_Lights; }

private:
    MaterialHandle m_Skybox;
    CubemapHandle m_SkyboxCubemap;
    Vec4 m_AmbientColor = {0.15f, 0.15f, 0.2f, 1.0f};
    std::vector<Light*> m_Lights;
    DirectionalLightData m_SunData;
};

} // namespace AMEE

#endif // __AMEE_SCENE_H__