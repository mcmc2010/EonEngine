#ifndef __AMEE_DEMOAPP_H__
#define __AMEE_DEMOAPP_H__
#pragma once
#include "AMEEFramework/Core/AMEEApplication.hpp"
#include "AMEEFramework/Platform/macOS/AMEEMacosApplication.hpp"
#include "AMEEFramework/Core/Asset/AMEEAssetHandle.hpp"
#include "AMEEFramework/Core/AMEECamera.hpp"
#include "AMEEFramework/Core/AMEEScene.hpp"
#include "AMEEFramework/Core/Entity/AMEEEntity.hpp"
#include "AMEEFramework/Core/Components/AMEEMeshFilter.hpp"
#include "AMEEFramework/Core/Components/AMEEMeshRenderer.hpp"
#include "AMEEFramework/Core/Components/AMEELight.hpp"
#include "AMEEFramework/Core/Components/AMEEGridHelper.hpp"
#include "AMEEFramework/Render/Material/AMEESkyboxMaterial.hpp"
#include "AMEEFramework/Core/Math/AMEEMath.hpp"
#include "AMEEFramework/Core/Log/AMEELog.hpp"
#include <memory>

namespace AMEE {

class DemoApp : public MacosApplication {
protected:
    bool OnInit() override;
    void OnFixedUpdate(float fixedDt) override;
    void OnRender(double deltaTime, double totalTime, double alpha) override;
    void OnShutdown() override;

private:
    ShaderHandle m_ShaderHandle;
    MeshHandle m_MeshHandle;
    std::vector<MaterialHandle> m_MaterialHandles;

    // Scene entities
    Entity* m_pCameraEntity = nullptr;
    Camera* m_pCamera = nullptr;
    Entity* m_pCubeEntity = nullptr;
    MeshRenderer* m_pCubeRenderer = nullptr;

    // Skybox
    Entity* m_pSkyboxEntity = nullptr;
    GridHelper* m_pGridHelper = nullptr;

    // Scene
    std::unique_ptr<Scene> m_pScene;
    ShaderProgram* m_pShader = nullptr;

    bool m_CaptureMouse = false;
};

} // namespace AMEE

#endif // __AMEE_DEMOAPP_H__