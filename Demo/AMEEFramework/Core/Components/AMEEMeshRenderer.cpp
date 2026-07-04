#include "AMEEMeshRenderer.hpp"
#include "AMEEMeshFilter.hpp"
#include "../Asset/AMEEAssetManager.hpp"
#include "../Entity/AMEEEntity.hpp"
#include "../../Render/AMEERHI.hpp"
#include "../../Render/AMEEMesh.hpp"
#include "../../Render/Shader/AMEEShaderProgram.hpp"
#include "../../Render/Texture/AMEETexture2D.hpp"

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
    ShaderProgram* Shader = Assets.GetShader(m_Shader);
    Texture2D* Tex = Assets.GetTexture(m_Texture);

    if (!Mesh || !Shader) return;

    Mat4 MVP = ViewProj * Owner->GetWorldMatrix();

    Shader->use();
    if (Tex) Tex->Bind(0);
    Shader->setInt("uTexture", 0);
    Shader->setMat4("uMVP", MVP.Data());

    Mesh->Draw();
}

} // namespace AMEE
