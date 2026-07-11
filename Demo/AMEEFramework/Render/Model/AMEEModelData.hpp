#ifndef __AMEE_MODELDATA_H__
#define __AMEE_MODELDATA_H__
#pragma once
#include <vector>
#include <cstdint>
#include <string>
#include "../AMEEMesh.hpp"

namespace AMEE {

struct ModelData {
    std::vector<float> Vertices;
    std::vector<uint32_t> Indices;
    VertexLayout Layout;
    std::vector<SubMesh> SubMeshes;      // index ranges into Vertices/Indices
    std::vector<std::string> MaterialNames; // usemtl names for each SubMesh
};

} // namespace AMEE

#endif // __AMEE_MODELDATA_H__
