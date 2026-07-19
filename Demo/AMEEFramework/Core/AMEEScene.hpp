#ifndef __AMEE_SCENE_H__
#define __AMEE_SCENE_H__
#pragma once

#include "AMEEDefines.hpp"
#include "AMEENode.hpp"
#include "AMEEObject.hpp"
#include "Asset/AMEEAssetHandle.hpp"
#include "Math/AMEEMath.hpp"
#include <vector>

namespace AMEE {

class Entity;
class Component;
class Light;
class ShaderProgram;
class RHI;
class Camera;

struct DirectionalLightData {
    Vec3 Direction;
    Vec4 Color;       // RGB + Intensity
};

class Scene : public Node, public Object {
    friend Entity;
public:
    Scene() = default;
    virtual ~Scene() = default;

    //
    virtual bool AddChild(std::unique_ptr<Node> Child) override;
    virtual std::unique_ptr<Node> RemoveChild(Node* Child) override;
    
    //
    Camera* GetMainCamera() const { return m_pMainCamera; }
    
    //
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

// 事件回调（protected，仅内部使用）
protected:
    virtual bool OnAddedChild(Entity* entity);
    virtual void OnRemovedChild(Entity* entity);
    virtual bool OnAddedComponent(Entity* entity, Component* comp);
    virtual void OnRemovedComponent(Entity* entity, Component* comp);
    
protected:
    // Render hooks (override in subclass for custom behavior)
    virtual void OnPreRender(RHI* rhi, const Mat4& ViewProj, Entity* entity) {}
    virtual void OnRender(RHI* rhi, const Mat4& ViewProj, Entity* entity);
    virtual void OnPostRender(RHI* rhi, const Mat4& ViewProj, Entity* entity) {}

    // Recursive render
    void RenderChildren(RHI* rhi, const Mat4& ViewProj, const std::vector<std::unique_ptr<Node>>& Children);

private:
    //
    Camera* m_pMainCamera = nullptr;
    
    //
    MaterialHandle m_Skybox;
    CubemapHandle m_SkyboxCubemap;
    Vec4 m_AmbientColor = {0.15f, 0.15f, 0.2f, 1.0f};
    std::vector<Light*> m_Lights;
    DirectionalLightData m_SunData;
};

} // namespace AMEE

#endif // __AMEE_SCENE_H__
