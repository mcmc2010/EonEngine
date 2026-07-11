#include "DemoApp.hpp"
#include "AMEEFramework/Core/Asset/AMEEAssetManager.hpp"
#include "AMEEFramework/Core/Platform/IAMEEPlatformInput.hpp"
#include "AMEEFramework/Core/Log/AMEELog.hpp"
#include "AMEEFramework/Render/AMEERHI.hpp"
#include "AMEEFramework/Render/AMEEMesh.hpp"
#include "AMEEFramework/Render/Shader/AMEEShaderProgram.hpp"
#include "AMEEFramework/Render/Texture/AMEETexture2D.hpp"
#include "AMEEFramework/Render/Material/AMEEStandardMaterial.hpp"
#include <functional>

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

    // Create scene
    m_pScene = std::make_unique<Scene>();
    m_pScene->SetName("DemoScene");
    m_pScene->SetAmbientColor({0.3f, 0.3f, 0.35f, 1.0f});

    // ─── Skybox (procedural — cubemap textures todo) ───────────────────────

    // ─── Model Entity (loaded from OBJ + MTL) ──────────────────────────────

    m_MeshHandle = assets.LoadModel(rhi, "Assets/Models/female01.obj", &m_MaterialHandles);
    if (!m_MeshHandle.IsValid()) {
        AMEE_LOG_ERROR("DemoApp", "Failed to load model");
        return false;
    }

    auto CubeEntity = std::make_unique<Entity>();
    CubeEntity->SetName("Female01");
    CubeEntity->SetPosition({0, 0, 0});
    CubeEntity->SetScale({0.02f, 0.02f, 0.02f});

    auto* Filter = CubeEntity->AddComponent<MeshFilter>();
    Filter->SetMesh(m_MeshHandle);

    m_pCubeRenderer = CubeEntity->AddComponent<MeshRenderer>();
    m_pCubeRenderer->m_Materials = m_MaterialHandles;

    // Set shader on all materials
    for (auto& H : m_MaterialHandles) {
        if (Material* M = assets.GetMaterial(H)) {
            M->SetShader(m_ShaderHandle);
        }
    }
    m_pCubeEntity = CubeEntity.get();

    m_pScene->AddChild(std::move(CubeEntity));

    // ─── Sun Light ─────────────────────────────────────────────────────────

    auto SunEntity = std::make_unique<Entity>();
    SunEntity->SetName("Sun");
    SunEntity->SetRotation({50, 30, 0});

    auto* LightComp = SunEntity->AddComponent<Light>();
    LightComp->m_Color = {1.0f, 0.95f, 0.85f, 1.2f};

    m_pScene->AddChild(std::move(SunEntity));

    // ─── Grid ──────────────────────────────────────────────────────────────

    auto GridEnt = std::make_unique<Entity>();
    GridEnt->SetName("Grid");
    m_pGridHelper = GridEnt->AddComponent<GridHelper>();
    m_pGridHelper->Create(rhi, 10.0f, 10);
    m_pScene->AddChild(std::move(GridEnt));

    // ─── Camera Entity ─────────────────────────────────────────────────────

    auto CameraEntity = std::make_unique<Entity>();
    CameraEntity->SetName("MainCamera");
    CameraEntity->SetPosition({0, 1.5f, 4});

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

void DemoApp::OnRender(double deltaTime, double totalTime, double alpha)
{
    RHI* rhi = GetRHI();

    rhi->setClearColor(m_pScene->GetAmbientColor().x,
                       m_pScene->GetAmbientColor().y,
                       m_pScene->GetAmbientColor().z, 1.0f);
    rhi->clear();

    int w, h;
    GetGLContext()->getSize(w, h);
    float aspect = (float)w / (float)h;

    Mat4 View = m_pCamera->GetViewMatrix();
    Mat4 Proj = m_pCamera->GetProjectionMatrix(aspect);
    Mat4 VP = Proj * View;

    auto& Assets = AssetManager::Instance();
    m_pShader = Assets.GetShader(m_ShaderHandle);
    m_pScene->ApplyLighting(m_pShader);

    // Grid
    if (m_pGridHelper && m_pShader) {
        Mat4 GridMVP = VP;
        m_pShader->use();
        m_pShader->setMat4("uMVP", GridMVP.Data());
        m_pGridHelper->Draw(rhi, VP);
    }

    // Iterate scene and render all MeshRenderers recursively
    std::function<void(const std::vector<std::unique_ptr<Node>>&)> RenderNodes =
        [&](const auto& Children) {
            for (auto& Child : Children) {
                if (auto* Ent = dynamic_cast<Entity*>(Child.get())) {
                    if (auto* Renderer = Ent->GetComponent<MeshRenderer>()) {
                        Renderer->Draw(rhi, VP);
                    }
                }
                RenderNodes(Child->GetChildren());
            }
        };
    RenderNodes(m_pScene->GetChildren());
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
    assets.UnloadMesh(m_MeshHandle);
    for (auto& H : m_MaterialHandles) assets.UnloadMaterial(H);

    AMEE_LOG_INFO("DemoApp", "Demo shutdown");
}

} // namespace AMEE
