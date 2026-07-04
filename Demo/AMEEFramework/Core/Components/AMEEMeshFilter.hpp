#ifndef __AMEE_MESHFILTER_H__
#define __AMEE_MESHFILTER_H__
#pragma once
#include "AMEEComponent.hpp"
#include "../Asset/AMEEAssetHandle.hpp"

namespace AMEE {

class MeshFilter : public Component {
public:
    MeshFilter() = default;

    void SetMesh(MeshHandle Handle) { m_Mesh = Handle; }
    MeshHandle GetMesh() const { return m_Mesh; }

private:
    MeshHandle m_Mesh;
};

} // namespace AMEE

#endif // __AMEE_MESHFILTER_H__
