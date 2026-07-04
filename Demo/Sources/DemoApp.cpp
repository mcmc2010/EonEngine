#include "DemoApp.hpp"
#include "AMEEFramework/Core/Asset/AMEEAssetManager.hpp"
#include "AMEEFramework/Core/Platform/IAMEEPlatformInput.hpp"
#include "AMEEFramework/Core/Log/AMEELog.hpp"
#include "AMEEFramework/Render/AMEERHI.hpp"
#include "AMEEFramework/Render/AMEEMesh.hpp"
#include "AMEEFramework/Render/Shader/AMEEShaderProgram.hpp"
#include "AMEEFramework/Render/Texture/AMEETexture2D.hpp"
#include "AMEEFramework/Core/Meshes/AMEEPrimitiveMesh.hpp"

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

    // Create scene
    m_pScene = std::make_unique<Scene>();
    m_pScene->SetName("DemoScene");

    // ─── Cube Entity ───────────────────────────────────────────────────────

    auto CubeMesh = std::unique_ptr<Mesh>(PrimitiveMesh::CreateCube(rhi, 1.0f));
    if (!CubeMesh) {
        AMEE_LOG_ERROR("DemoApp", "Failed to create cube mesh");
        return false;
    }
    m_MeshHandle = assets.RegisterMesh(std::move(CubeMesh), "Cube");

    auto CubeEntity = std::make_unique<Entity>();
    CubeEntity->SetName("Cube");
    CubeEntity->SetPosition({0, 0, 0});

    auto* Filter = CubeEntity->AddComponent<MeshFilter>();
    Filter->SetMesh(m_MeshHandle);

    m_pCubeRenderer = CubeEntity->AddComponent<MeshRenderer>();
    m_pCubeRenderer->m_Shader = m_ShaderHandle;
    m_pCubeRenderer->m_Texture = m_TextureHandle;
    m_pCubeEntity = CubeEntity.get();

    m_pScene->AddChild(std::move(CubeEntity));

    // ─── Camera Entity ─────────────────────────────────────────────────────

    auto CameraEntity = std::make_unique<Entity>();
    CameraEntity->SetName("MainCamera");
    CameraEntity->SetPosition({0, 0, 3});

    m_pCamera = CameraEntity->AddComponent<Camera>(60.0f, 0.1f, 1000.0f);
    m_pCamera->SetRotation(-90.0f, 0);
    m_pCameraEntity = CameraEntity.get();

    m_pScene->AddChild(std::move(CameraEntity));

    AMEE_LOG_INFO("DemoApp", "Scene '%s' ready (%zu entities)",
                  m_pScene->GetName().c_str(), m_pScene->GetChildCount());
    return true;
}

void DemoApp::OnFixedUpdate(float fixedDt)
{
    IPlatformInput* input = GetInput();
    if (!input || !m_pCamera || !m_pCameraEntity) return;

    float MoveSpeed = 3.0f;
    float LookSpeed = 0.15f;

    if (input->IsMouseButtonPressed(MouseButton::Right)) {
        m_CaptureMouse = true;
    }
    if (input->IsMouseButtonReleased(MouseButton::Right)) {
        m_CaptureMouse = false;
    }

    if (m_CaptureMouse) {
        float dx, dy;
        input->GetMouseDelta(dx, dy);
        m_pCamera->Rotate(dx * LookSpeed, -dy * LookSpeed);
    }

    Vec3 Pos = m_pCameraEntity->GetPosition();
    if (input->IsKeyDown(KeyCode::W)) Pos = Pos + m_pCamera->GetForward() * (MoveSpeed * fixedDt);
    if (input->IsKeyDown(KeyCode::S)) Pos = Pos + m_pCamera->GetForward() * (-MoveSpeed * fixedDt);
    if (input->IsKeyDown(KeyCode::A)) Pos = Pos + m_pCamera->GetRight() * (-MoveSpeed * fixedDt);
    if (input->IsKeyDown(KeyCode::D)) Pos = Pos + m_pCamera->GetRight() * (MoveSpeed * fixedDt);
    if (input->IsKeyDown(KeyCode::Q)) Pos.y -= MoveSpeed * fixedDt;
    if (input->IsKeyDown(KeyCode::E)) Pos.y += MoveSpeed * fixedDt;
    m_pCameraEntity->SetPosition(Pos);

    m_pScene->Update(fixedDt);

    // Spin the cube so we can see it's 3D
    Vec3 Rot = m_pCubeEntity->GetRotation();
    Rot.y += fixedDt * 45.0f;
    m_pCubeEntity->SetRotation(Rot);
}

void DemoApp::OnRender(double deltaTime, double totalTime)
{
    RHI* rhi = GetRHI();
    auto& assets = AssetManager::Instance();

    rhi->setClearColor(0.15f, 0.15f, 0.2f, 1.0f);
    rhi->clear();

    ShaderProgram* shader = assets.GetShader(m_ShaderHandle);
    if (!shader) return;

    int w, h;
    GetGLContext()->getSize(w, h);
    float aspect = (float)w / (float)h;

    Mat4 View = m_pCamera->GetViewMatrix();
    Mat4 Proj = m_pCamera->GetProjectionMatrix(aspect);
    Mat4 VP = Proj * View;

    // Iterate scene and render all MeshRenderers
    for (auto& Child : m_pScene->GetChildren()) {
        if (auto* Ent = dynamic_cast<Entity*>(Child.get())) {
            if (auto* Renderer = Ent->GetComponent<MeshRenderer>()) {
                Renderer->Draw(rhi, VP);
            }
        }
    }
}

void DemoApp::OnShutdown()
{
    m_pScene.reset();
    m_pCameraEntity = nullptr;
    m_pCamera = nullptr;
    m_pCubeEntity = nullptr;
    m_pCubeRenderer = nullptr;

    auto& assets = AssetManager::Instance();
    assets.UnloadShader(m_ShaderHandle);
    assets.UnloadTexture(m_TextureHandle);
    assets.UnloadMesh(m_MeshHandle);

    AMEE_LOG_INFO("DemoApp", "Demo shutdown");
}

} // namespace AMEE
