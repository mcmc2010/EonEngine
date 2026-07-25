#ifndef __AMEE_APPLICATION_H__
#define __AMEE_APPLICATION_H__
#pragma once
#include "Platform/IAMEEPlatformWindow.hpp"
#include "Platform/IAMEEPlatformContext.hpp"
#include "Platform/IAMEEPlatformLoop.hpp"
#include "Platform/IAMEEPlatformInput.hpp"
#include "AMEERenderPipeline.hpp"
#include "../Render/AMEERHI.hpp"
#include "../Render/Shader/AMEEShaderProgram.hpp"
#include <memory>
#include <string>

namespace AMEE {

struct ApplicationConfig {
    int Width = 1024;
    int Height = 768;
    std::string Title = "AMEE Engine";
    int MinWidth = 320;
    int MinHeight = 240;
};

class Application {
public:
    Application() = default;
    virtual ~Application() = default;

    // Lifecycle
    bool Init(const ApplicationConfig& config);
    void Shutdown();
    void Run();

    // Accessors
    IPlatformWindow* GetWindow() const { return m_pWindow.get(); }
    IPlatformContext* GetContext() const { return m_pContext.get(); }
    IPlatformLoop* GetGameLoop() const { return m_pGameLoop.get(); }
    IPlatformInput* GetInput() const { return m_pInput.get(); }
    RHI* GetRHI() const { return m_pRHI.get(); }
    RenderPipeline* GetPipeline() const { return m_pPipeline.get(); }

protected:
    // Override in subclass for custom init/render/shutdown
    virtual bool OnInit() { return true; }
    virtual void OnFixedUpdate(float fixedDt) {}
    virtual void OnRender(double deltaTime, double totalTime, double alpha) = 0;
    virtual void OnShutdown() {}
    virtual void OnWindowShouldClose() {}

    // Platform factory methods — override per platform
    virtual std::unique_ptr<IPlatformWindow> CreatePlatformWindow() = 0;
    virtual std::unique_ptr<IPlatformContext> CreatePlatformContext() = 0;
    virtual std::unique_ptr<IPlatformLoop> CreatePlatformGameLoop() = 0;
    virtual std::unique_ptr<IPlatformInput> CreatePlatformInput() = 0;
    virtual std::unique_ptr<RHI> CreateRHI() = 0;

private:
    std::unique_ptr<IPlatformWindow> m_pWindow;
    std::unique_ptr<IPlatformContext> m_pContext;
    std::unique_ptr<IPlatformLoop> m_pGameLoop;
    std::unique_ptr<IPlatformInput> m_pInput;
    std::unique_ptr<RHI> m_pRHI;
    std::unique_ptr<RenderPipeline> m_pPipeline;
    bool m_Running = false;
};

} // namespace AMEE

#endif // __AMEE_APPLICATION_H__
