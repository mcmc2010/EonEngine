#include "AMEEMeshRenderer.hpp"
#include "AMEEMeshFilter.hpp"
#include "../Asset/AMEEAssetManager.hpp"
#include "../Entity/AMEEEntity.hpp"
#include "../../Render/AMEERHI.hpp"
#include "../../Render/AMEEMesh.hpp"
#include "../../Render/Material/AMEEMaterial.hpp"
#include "../../Render/Shader/AMEEShaderProgram.hpp"

namespace AMEE {

// Compute normal matrix = transpose(inverse(mat3(model)))
// This is needed for correct lighting with non-uniform scaling
static void ComputeNormalMatrix(const Mat4& Model, float Out[9])
{
    // Extract 3x3 from 4x4 model matrix
    float a = Model.at(0, 0), b = Model.at(0, 1), c = Model.at(0, 2);
    float d = Model.at(1, 0), e = Model.at(1, 1), f = Model.at(1, 2);
    float g = Model.at(2, 0), h = Model.at(2, 1), i = Model.at(2, 2);

    // Compute determinant of 3x3
    float det = a * (e * i - f * h)
              - b * (d * i - f * g)
              + c * (d * h - e * g);

    if (std::abs(det) < 1e-6f) {
        // Singular matrix, return identity
        Out[0] = 1; Out[1] = 0; Out[2] = 0;
        Out[3] = 0; Out[4] = 1; Out[5] = 0;
        Out[6] = 0; Out[7] = 0; Out[8] = 1;
        return;
    }

    float invDet = 1.0f / det;

    // Inverse of 3x3, then transpose
    Out[0] = (e * i - f * h) * invDet;
    Out[3] = -(b * i - c * h) * invDet;
    Out[6] = (b * f - c * e) * invDet;

    Out[1] = -(d * i - f * g) * invDet;
    Out[4] = (a * i - c * g) * invDet;
    Out[7] = -(a * f - c * d) * invDet;

    Out[2] = (d * h - e * g) * invDet;
    Out[5] = -(a * h - b * g) * invDet;
    Out[8] = (a * e - b * d) * invDet;
}

void MeshRenderer::Draw(RHI* rhi, const Mat4& ViewProj)
{
    if (!m_Visible || !rhi) return;

    Entity* Owner = GetOwner();
    if (!Owner) return;

    MeshFilter* Filter = Owner->GetComponent<MeshFilter>();
    if (!Filter || !Filter->GetMesh().IsValid()) return;

    auto& Assets = AssetManager::GetSingleton();
    Mesh* Mesh = Assets.GetMesh(Filter->GetMesh());
    if (!Mesh) return;

    Mat4 Model = Owner->GetWorldMatrix();
    Mat4 MVP = ViewProj * Model;

    // Pre-compute normal matrix on CPU
    float NormalMatrix[9];
    ComputeNormalMatrix(Model, NormalMatrix);

    int SubCount = Mesh->GetSubMeshCount();
    if (SubCount == 0) {
        // No submeshes — single material on whole mesh
        if (!m_Materials.empty()) {
            Material* Mat = Assets.GetMaterial(m_Materials[0]);
            if (Mat) {
                Mat->Apply(rhi);
                if (ShaderProgram* S = Assets.GetShader(Mat->GetShader())) {
                    S->setMat4("u_MVP", MVP.Data());
                    S->setMat4("u_Model", Model.Data());
                    S->setMat3("u_NormalMatrix", NormalMatrix);
                }
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
                        S->setMat4("u_MVP", MVP.Data());
                        S->setMat4("u_Model", Model.Data());
                        S->setMat3("u_NormalMatrix", NormalMatrix);
                    }
                }
            }
            Mesh->DrawSubMesh(I);
        }
    }
}

} // namespace AMEE