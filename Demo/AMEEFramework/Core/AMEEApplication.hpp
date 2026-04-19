#ifndef __AMEE_APPLICATION_H__
#define __AMEE_APPLICATION_H__
#pragma once
#include "Platform/IAMEEPlatformWindow.hpp"
#include "Platform/IAMEEPlatformGLContext.hpp"
#include "Platform/IAMEEPlatformLoop.hpp"
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
    IPlatformGLContext* GetGLContext() const { return m_pGLContext.get(); }
    IPlatformLoop* GetGameLoop() const { return m_pGameLoop.get(); }
    RHI* GetRHI() const { return m_pRHI.get(); }

protected:
    // Override in subclass for custom init/render/shutdown
    virtual bool OnInit() { return true; }
    virtual void OnRender(float deltaTime, float totalTime) = 0;
    virtual void OnShutdown() {}

    // Platform factory methods — override per platform
    virtual std::unique_ptr<IPlatformWindow> CreatePlatformWindow() = 0;
    virtual std::unique_ptr<IPlatformGLContext> CreatePlatformGLContext() = 0;
    virtual std::unique_ptr<IPlatformLoop> CreatePlatformGameLoop() = 0;
    virtual std::unique_ptr<RHI> CreateRHI() = 0;

private:
    std::unique_ptr<IPlatformWindow> m_pWindow;
    std::unique_ptr<IPlatformGLContext> m_pGLContext;
    std::unique_ptr<IPlatformLoop> m_pGameLoop;
    std::unique_ptr<RHI> m_pRHI;
    bool m_Running = false;
};

} // namespace AMEE

#endif // __AMEE_APPLICATION_H__
