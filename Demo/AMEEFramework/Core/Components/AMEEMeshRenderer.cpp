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
    if (!m_Visible || !rhi) return;

    Entity* Owner = GetOwner();
    if (!Owner) return;

    MeshFilter* Filter = Owner->GetComponent<MeshFilter>();
    if (!Filter || !Filter->GetMesh().IsValid()) return;

    auto& Assets = AssetManager::Instance();
    Mesh* Mesh = Assets.GetMesh(Filter->GetMesh());
    if (!Mesh) return;

    Mat4 MVPBase = ViewProj * Owner->GetWorldMatrix();

    int SubCount = Mesh->GetSubMeshCount();
    if (SubCount == 0) {
        // No submeshes — single material on whole mesh
        if (!m_Materials.empty()) {
            Material* Mat = Assets.GetMaterial(m_Materials[0]);
            if (Mat) {
                Mat->Apply(rhi);
                Assets.GetShader(Mat->GetShader())->setMat4("uMVP", MVPBase.Data());
            }
        }
        Mesh->Draw();
    } else {
        for (int I = 0; I < SubCount; I++) {
            if (I < (int)m_Materials.size()) {
                Material* Mat = Assets.GetMaterial(m_Materials[I]);
                if (Mat) {
                    Mat->Apply(rhi);
                    if (ShaderProgram* S = Assets.GetShader(Mat->GetShader())) {
                        S->setMat4("uMVP", MVPBase.Data());
                    }
                }
            }
            Mesh->DrawSubMesh(I);
        }
    }
}

} // namespace AMEE
