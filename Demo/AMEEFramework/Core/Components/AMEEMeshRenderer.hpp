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



    
    //
    void SetMaterial(MaterialHandle H) { m_Materials = {H}; }
    void Draw(RHI* rhi, const Mat4& ViewProj);
    
protected:
    //
    bool m_CastsShadow = true;    // 投射阴影
    bool m_ReceivesShadow = true; // 接收阴影
};

} // namespace AMEE

#endif // __AMEE_MESHRENDERER_H__
