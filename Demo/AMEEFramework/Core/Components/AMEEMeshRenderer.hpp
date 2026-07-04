#ifndef __AMEE_MESHRENDERER_H__
#define __AMEE_MESHRENDERER_H__
#pragma once
#include "AMEEComponent.hpp"
#include "../Asset/AMEEAssetHandle.hpp"
#include "../Math/AMEEMath.hpp"

namespace AMEE {

class RHI;

class MeshRenderer : public Component {
public:
    MeshRenderer() = default;

    ShaderHandle m_Shader;
    TextureHandle m_Texture;
    bool m_Visible = true;

    void Draw(RHI* rhi, const Mat4& ViewProj);
};

} // namespace AMEE

#endif // __AMEE_MESHRENDERER_H__
