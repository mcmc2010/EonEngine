#ifndef __AMEE_DEMOAPP_H__
#define __AMEE_DEMOAPP_H__
#pragma once
#include "AMEEFramework/Core/AMEEApplication.hpp"
#include "AMEEFramework/Platform/macOS/AMEEMacosApplication.hpp"
#include "AMEEFramework/Core/Asset/AMEEAssetHandle.hpp"
#include "AMEEFramework/Core/AMEECamera.hpp"
#include "AMEEFramework/Render/AMEEMesh.hpp"
#include "AMEEFramework/Core/Math/AMEEMath.hpp"
#include "AMEEFramework/Core/Log/AMEELog.hpp"
#include <memory>

namespace AMEE {

class DemoApp : public MacosApplication {
protected:
    bool OnInit() override;
    void OnFixedUpdate(float fixedDt) override;
    void OnRender(double deltaTime, double totalTime) override;
    void OnShutdown() override;

private:
    ShaderHandle m_ShaderHandle;
    std::unique_ptr<Mesh> m_pQuad;
    TextureHandle m_TextureHandle;
    Camera m_Camera;
    double m_Angle = 0;
    bool m_CaptureMouse = false;
};

} // namespace AMEE

#endif // __AMEE_DEMOAPP_H__
