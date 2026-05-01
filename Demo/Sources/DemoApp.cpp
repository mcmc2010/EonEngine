#include "DemoApp.hpp"
#include "AMEEFramework/Render/Shader/GL/AMEEGLShader.hpp"
#include "AMEEFramework/Core/Log/AMEELog.hpp"

namespace AMEE {

bool DemoApp::OnInit()
{
    // Create shader
    m_pShader = std::make_unique<GLShaderProgram>();

    const char* vs = R"(
        #version 410 core
        layout(location = 0) in vec3 aPos;
        layout(location = 2) in vec2 aTexCoord;
        uniform mat4 uMVP;
        out vec2 vTexCoord;
        void main() {
            gl_Position = uMVP * vec4(aPos, 1.0);
            vTexCoord = aTexCoord;
        }
    )";

    const char* fs = R"(
        #version 410 core
        in vec2 vTexCoord;
        uniform sampler2D uTexture;
        out vec4 fragColor;
        void main() {
            fragColor = texture(uTexture, vTexCoord);
        }
    )";

    m_pShader->compileFromSource(ShaderType::Vertex, vs, [](const ShaderCompileError& err) {
        AMEE_LOG_ERROR("Shader", "Vertex compile error: %s", err.message.c_str());
    });

    m_pShader->compileFromSource(ShaderType::Fragment, fs, [](const ShaderCompileError& err) {
        AMEE_LOG_ERROR("Shader", "Fragment compile error: %s", err.message.c_str());
    });

    if (!m_pShader->link([](const std::string& err) {
        AMEE_LOG_ERROR("Shader", "Link error: %s", err.c_str());
    })) {
        AMEE_LOG_ERROR("Shader", "Failed to create shader");
        return false;
    }

    // Load texture
    m_pTexture = std::make_unique<Texture2D>();
    if (!m_pTexture->Load(GetRHI(), "Assets/Textures/03.png")) {
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
    quadLayout.Add(0, 3, RHIDataType::Float)    // position
              .Add(2, 2, RHIDataType::Float);   // texcoord (location=2)

    m_pQuad = std::make_unique<Mesh>();
    if (!m_pQuad->CreateIndexed(GetRHI(), quadVertices, 4, quadIndices, 6, quadLayout)) {
        AMEE_LOG_ERROR("DemoApp", "Failed to create quad mesh");
        return false;
    }

    AMEE_LOG_INFO("DemoApp", "Demo initialized (textured quad)");
    return true;
}

void DemoApp::OnRender(float deltaTime, float totalTime)
{
    RHI* rhi = GetRHI();

    rhi->setClearColor(0.15f, 0.15f, 0.2f, 1.0f);
    rhi->clear();

    // Bind texture to slot 0
    m_pTexture->Bind(0);

    float t = totalTime;
    Mat4 model = Mat4::RotateY(t * 45.0f);

    int w, h;
    GetGLContext()->getSize(w, h);
    float aspect = (float)w / (float)h;

    Mat4 view = Mat4::LookAt({0.0f, 0.0f, 2.5f},
                              {0.0f, 0.0f, 0.0f},
                              {0.0f, 1.0f, 0.0f});
    Mat4 proj = Mat4::Perspective(45.0f, aspect, 0.1f, 100.0f);
    Mat4 mvp = proj * view * model;

    m_pShader->use();
    m_pShader->setMat4("uMVP", mvp.Data());
    m_pShader->setInt("uTexture", 0);

    m_pQuad->Draw();
}

void DemoApp::OnShutdown()
{
    m_pQuad.reset();
    m_pTexture.reset();
    m_pShader.reset();
    AMEE_LOG_INFO("DemoApp", "Demo shutdown");
}

} // namespace AMEE