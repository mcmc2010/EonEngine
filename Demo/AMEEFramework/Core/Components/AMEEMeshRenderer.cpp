#include "AMEEMeshRenderer.hpp"
#include "AMEEMeshFilter.hpp"
#include "../Asset/AMEEAssetManager.hpp"
#include "../Entity/AMEEEntity.hpp"
#include "../../Render/AMEERHI.hpp"
#include "../../Render/AMEEMesh.hpp"
#include "../../Render/Material/AMEEMaterial.hpp"
#include "../../Render/Shader/AMEEShaderProgram.hpp"

namespace AMEE {

void MeshRenderer::Draw(RHI* rhi, const Mat4& ViewProj)
{
    if (!m_Visible || !rhi || !m_Material.IsValid()) return;

    Entity* Owner = GetOwner();
    if (!Owner) return;

    MeshFilter* Filter = Owner->GetComponent<MeshFilter>();
    if (!Filter || !Filter->GetMesh().IsValid()) return;

    auto& Assets = AssetManager::Instance();
    Mesh* Mesh = Assets.GetMesh(Filter->GetMesh());
    Material* Mat = Assets.GetMaterial(m_Material);

    if (!Mesh || !Mat) return;

    Mat4 MVP = ViewProj * Owner->GetWorldMatrix();

    Mat->Apply(rhi);
    Assets.GetShader(Mat->GetShader())->setMat4("uMVP", MVP.Data());

    Mesh->Draw();
}

} // namespace AMEE
