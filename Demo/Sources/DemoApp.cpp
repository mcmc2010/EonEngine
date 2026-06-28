#include "DemoApp.hpp"
#include "AMEEFramework/Core/Asset/AMEEAssetManager.hpp"
#include "AMEEFramework/Core/Log/AMEELog.hpp"
#include "AMEEFramework/Render/AMEERHI.hpp"
#include "AMEEFramework/Render/Shader/AMEEShaderProgram.hpp"
#include "AMEEFramework/Render/Texture/AMEETexture2D.hpp"

namespace AMEE {

bool DemoApp::OnInit()
{
    RHI* rhi = GetRHI();
    auto& assets = AssetManager::Instance();

    // Load shader from files
    m_ShaderHandle = assets.LoadShader(rhi, "Assets/Shaders/Default.vert", "Assets/Shaders/Default.frag");
    if (!m_ShaderHandle.IsValid()) {
        AMEE_LOG_ERROR("DemoApp", "Failed to load shader");
        return false;
    }

    // Load texture
    m_TextureHandle = assets.LoadTexture(rhi, "Assets/Textures/04.png");
    if (!m_TextureHandle.IsValid()) {
        AMEE_LOG_ERROR("DemoApp", "Failed to load texture");
        return false;
    }

    // Create quad with UV coordinates
    float quadVertices[] = {
        // position (x,y,z)      // texcoord (u,v)
        -0.8f, -0.8f, 0.0f,      0.0f, 0.0f,
         0.8f, -0.8f, 0.0f,      1.0f, 0.0f,
         0.8f,  0.8f, 0.0f,      1.0f, 1.0f,
        -0.8f,  0.8f, 0.0f,      0.0f, 1.0f,
    };
    uint32_t quadIndices[] = { 0, 1, 2, 0, 2, 3 };

    VertexLayout quadLayout;
    quadLayout.Add(0, 3, RHIDataType::Float)
              .Add(2, 2, RHIDataType::Float);

    m_pQuad = std::make_unique<Mesh>();
    if (!m_pQuad->CreateIndexed(rhi, quadVertices, 4, quadIndices, 6, quadLayout)) {
        AMEE_LOG_ERROR("DemoApp", "Failed to create quad mesh");
        return false;
    }

    AMEE_LOG_INFO("DemoApp", "Demo initialized (textured quad, %zu textures, %zu shaders)",
                  assets.GetTextureCount(), assets.GetShaderCount());
    return true;
}

void DemoApp::OnFixedUpdate(float fixedDt)
{
    static int s_TickCount = 0;
    s_TickCount++;
    if (s_TickCount % 60 == 0) {
        AMEE_LOG_INFO("DemoApp", "FixedUpdate: %d ticks (%.2f dt)",
                      s_TickCount, fixedDt);
    }
}

void DemoApp::OnRender(double deltaTime, double totalTime)
{
    RHI* rhi = GetRHI();
    auto& assets = AssetManager::Instance();

    rhi->setClearColor(0.15f, 0.15f, 0.2f, 1.0f);
    rhi->clear();

    Texture2D* tex = assets.GetTexture(m_TextureHandle);
    ShaderProgram* shader = assets.GetShader(m_ShaderHandle);
    if (!tex || !shader) return;

    tex->Bind(0);

    m_Angle += deltaTime * 45.0;
    Mat4 model = Mat4::RotateY((float)m_Angle);

    int w, h;
    GetGLContext()->getSize(w, h);
    float aspect = (float)w / (float)h;

    Mat4 view = Mat4::LookAt({0.0f, 0.0f, 2.5f},
                              {0.0f, 0.0f, 0.0f},
                              {0.0f, 1.0f, 0.0f});
    Mat4 proj = Mat4::Perspective(45.0f, aspect, 0.1f, 100.0f);
    Mat4 mvp = proj * view * model;

    shader->use();
    shader->setMat4("uMVP", mvp.Data());
    shader->setInt("uTexture", 0);

    m_pQuad->Draw();
}

void DemoApp::OnShutdown()
{
    m_pQuad.reset();

    auto& assets = AssetManager::Instance();
    assets.UnloadShader(m_ShaderHandle);
    assets.UnloadTexture(m_TextureHandle);

    AMEE_LOG_INFO("DemoApp", "Demo shutdown");
}

} // namespace AMEE
