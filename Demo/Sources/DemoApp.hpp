#ifndef __AMEE_DEMOAPP_H__
#define __AMEE_DEMOAPP_H__
#pragma once
#include "AMEEFramework/Core/AMEEApplication.hpp"
#include "AMEEFramework/Platform/macOS/AMEEMacosApplication.hpp"
#include "AMEEFramework/Render/AMEEMesh.hpp"
#include "AMEEFramework/Render/Shader/AMEEShaderProgram.hpp"
#include "AMEEFramework/Core/Math/AMEEMath.hpp"
#include "AMEEFramework/Core/Log/AMEELog.hpp"
#include <memory>

namespace AMEE {

class DemoApp : public MacosApplication {
protected:
    bool OnInit() override;
    void OnRender(float deltaTime, float totalTime) override;
    void OnShutdown() override;

private:
    std::unique_ptr<ShaderProgram> m_pShader;
    std::unique_ptr<Mesh> m_pTriangle;
};

} // namespace AMEE

#endif // __AMEE_DEMOAPP_H__
