#include "AMEEApplication.hpp"
#include "Log/AMEELog.hpp"

namespace AMEE {

bool Application::Init(const ApplicationConfig& config)
{
    AMEE::Logger::init(AMEE::LogLevel::Debug);

    // Create platform window
    m_pWindow = CreatePlatformWindow();
    if (!m_pWindow) {
        AMEE_LOG_ERROR("Application", "Failed to create platform window");
        return false;
    }
    m_pWindow->create(config.Width, config.Height, config.Title.c_str());
    m_pWindow->setMinSize(config.MinWidth, config.MinHeight);
    m_pWindow->center();
    m_pWindow->show();

    // Create GL context
    m_pGLContext = CreatePlatformGLContext();
    if (!m_pGLContext) {
        AMEE_LOG_ERROR("Application", "Failed to create platform GL context");
        return false;
    }
    if (!m_pGLContext->create(m_pWindow->getNativeHandle())) {
        AMEE_LOG_ERROR("Application", "Failed to create GL context");
        return false;
    }
    m_pGLContext->makeCurrent();

    // Create RHI
    m_pRHI = CreateRHI();
    if (!m_pRHI) {
        AMEE_LOG_ERROR("Application", "Failed to create RHI");
        return false;
    }

    // User-defined init
    if (!OnInit()) {
        AMEE_LOG_ERROR("Application", "OnInit failed");
        return false;
    }

    // Create game loop
    m_pGameLoop = CreatePlatformGameLoop();
    if (!m_pGameLoop) {
        AMEE_LOG_ERROR("Application", "Failed to create game loop");
        return false;
    }

    m_Running = true;
    AMEE_LOG_INFO("Application", "Initialized (%dx%d)", config.Width, config.Height);
    return true;
}

void Application::Shutdown()
{
    OnShutdown();

    m_pGameLoop->stop();
    m_pGLContext->makeCurrent();

    m_pRHI.reset();
    m_pGLContext.reset();
    m_pWindow.reset();

    AMEE_LOG_INFO("Application", "Shutdown complete");
    AMEE::Logger::flush();
}

void Application::Run()
{
    m_pGameLoop->start([this](double dt, double time) {
        m_pGLContext->makeCurrent();

        int w, h;
        m_pGLContext->getSize(w, h);
        m_pRHI->setViewport({0.0f, 0.0f, (float)w, (float)h});

        OnRender((float)dt, (float)time);

        m_pGLContext->swapBuffers();

        if (!m_pWindow->pollEvents()) {
            m_Running = false;
        }
    });
}

} // namespace AMEE
