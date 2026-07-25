#include "AMEEApplication.hpp"
#include "Log/AMEELog.hpp"
#include "Asset/AMEEFileSystem.hpp"
#include "Asset/AMEEAssetManager.hpp"
#include "../Render/Texture/AMEEImage.hpp"

namespace AMEE {

bool Application::Init(const ApplicationConfig& config)
{
    AMEE::Logger::init(AMEE::LogLevel::Debug);

    // Initialize virtual file system (bundle + cwd mount points)
    FileSystem::GetSingleton().Initialize();
    FileSystem::GetSingleton().PrintMounts();

    // Create platform window
    m_pWindow = CreatePlatformWindow();
    if (!m_pWindow) {
        AMEE_LOG_ERROR("Application", "Failed to create platform window");
        return false;
    }
    m_pWindow->create(config.Width, config.Height, config.Title.c_str());
    m_pWindow->setMinSize(config.MinWidth, config.MinHeight);
    m_pWindow->center();

    // Create GL context BEFORE showing window
    m_pContext = CreatePlatformContext();
    if (!m_pContext) {
        AMEE_LOG_ERROR("Application", "Failed to create platform GL context");
        return false;
    }
    if (!m_pContext->create(m_pWindow->getNativeHandle())) {
        AMEE_LOG_ERROR("Application", "Failed to create GL context");
        return false;
    }
    m_pContext->makeCurrent();

    // Show window AFTER GL context is set up
    m_pWindow->show();

    // Create RHI
    m_pRHI = CreateRHI();
    if (!m_pRHI) {
        AMEE_LOG_ERROR("Application", "Failed to create RHI");
        return false;
    }

    // Initialize RHI (enable seamless cubemap, etc.)
    m_pRHI->init();

    // Create render pipeline
    m_pPipeline = std::make_unique<RenderPipeline>(m_pRHI.get());

    // Default image flip for 2D textures
    SetImageFlipVertical(true);

    // Initialize built-in resources (textures, shaders, materials)
    AssetManager::GetSingleton().InitializeBuiltins(m_pRHI.get());

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

    // Create input
    m_pInput = CreatePlatformInput();
    if (!m_pInput) {
        AMEE_LOG_ERROR("Application", "Failed to create input");
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
    m_pContext->makeCurrent();

    AssetManager::GetSingleton().UnloadAll();

    m_pInput.reset();
    m_pRHI.reset();
    m_pContext.reset();
    m_pWindow.reset();

    AMEE_LOG_INFO("Application", "Shutdown complete");
    AMEE::Logger::flushAll();
}

void Application::Run()
{
    m_pGameLoop->start(
        // Render callback
        [this](double dt, double totalTime, double alpha) {
            m_pInput->Update();

            m_pContext->makeCurrent();

            int w, h;
            m_pContext->getSize(w, h);
            m_pRHI->setViewport({0.0f, 0.0f, (float)w, (float)h});

            OnRender(dt, totalTime, alpha);

            m_pContext->swapBuffers();

            if (!m_pWindow->pollEvents()) {
                m_Running = false;
                OnWindowShouldClose();
            }
        },
        // Fixed update callback
        [this](double fixedDt) {
            OnFixedUpdate((float)fixedDt);
        }
    );
}

} // namespace AMEE
