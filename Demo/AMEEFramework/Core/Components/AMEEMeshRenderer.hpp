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

    // Material
    void SetMaterial(MaterialHandle H) { m_Materials = {H}; }
    void Draw(RHI* rhi, const Mat4& ViewProj);

    // Shadow
    bool CastsShadow() const { return m_CastsShadow; }
    void SetCastsShadow(bool Value) { m_CastsShadow = Value; }
    bool ReceivesShadow() const { return m_ReceivesShadow; }
    void SetReceivesShadow(bool Value) { m_ReceivesShadow = Value; }

protected:
    bool m_CastsShadow = true;
    bool m_ReceivesShadow = true;
};

} // namespace AMEE

#endif // __AMEE_MESHRENDERER_H__