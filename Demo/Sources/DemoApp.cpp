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
        layout(location = 1) in vec3 aColor;
        uniform mat4 uMVP;
        out vec3 vColor;
        void main() {
            gl_Position = uMVP * vec4(aPos, 1.0);
            vColor = aColor;
        }
    )";

    const char* fs = R"(
        #version 410 core
        in vec3 vColor;
        out vec4 fragColor;
        void main() {
            fragColor = vec4(vColor, 1.0);
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
        AMEE_LOG_ERROR("Shader", "Failed to create triangle shader");
        return false;
    }

    // Create mesh using VertexLayout + Mesh abstraction
    float vertices[] = {
        -0.5f, -0.5f, 0.0f,   1.0f, 0.0f, 0.0f,
         0.5f, -0.5f, 0.0f,   0.0f, 1.0f, 0.0f,
         0.0f,  0.5f, 0.0f,   0.0f, 0.0f, 1.0f,
    };

    VertexLayout layout;
    layout.Add(0, 3, RHIDataType::Float)    // position (vec3)
          .Add(1, 3, RHIDataType::Float);   // color (vec3)

    m_pTriangle = std::make_unique<Mesh>();
    if (!m_pTriangle->Create(GetRHI(), vertices, 3, layout)) {
        AMEE_LOG_ERROR("DemoApp", "Failed to create triangle mesh");
        return false;
    }

    AMEE_LOG_INFO("DemoApp", "Demo initialized");
    return true;
}

void DemoApp::OnRender(float deltaTime, float totalTime)
{
    RHI* rhi = GetRHI();

    rhi->setClearColor(0.15f, 0.15f, 0.2f, 1.0f);
    rhi->clear();

    float t = totalTime;
    Mat4 model = Mat4::RotateY(t * 45.0f);

    int w, h;
    GetGLContext()->getSize(w, h);
    float aspect = (float)w / (float)h;

    Mat4 view = Mat4::LookAt(
        {0.0f, 1.0f, 3.0f},
        {0.0f, 0.0f, 0.0f},
        {0.0f, 1.0f, 0.0f}
    );

    Mat4 proj = Mat4::Perspective(45.0f, aspect, 0.1f, 100.0f);
    Mat4 mvp = proj * view * model;

    m_pShader->use();
    m_pShader->setMat4("uMVP", mvp.Data());

    m_pTriangle->Draw();
}

void DemoApp::OnShutdown()
{
    m_pTriangle.reset();
    m_pShader.reset();
    AMEE_LOG_INFO("DemoApp", "Demo shutdown");
}

} // namespace AMEE
