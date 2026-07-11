#ifndef __AMEE_MESHRENDERER_H__
#define __AMEE_MESHRENDERER_H__
#pragma once
#include "AMEEComponent.hpp"
#include "../Asset/AMEEAssetHandle.hpp"
#include "../Math/AMEEMath.hpp"
#include <vector>

namespace AMEE {

class RHI;

class MeshRenderer : public Component {
public:
    MeshRenderer() = default;

    std::vector<MaterialHandle> m_Materials;
    bool m_Visible = true;

    void SetMaterial(MaterialHandle H) { m_Materials = {H}; }
    void Draw(RHI* rhi, const Mat4& ViewProj);
};

} // namespace AMEE

#endif // __AMEE_MESHRENDERER_H__
