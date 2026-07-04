#ifndef __AMEE_MODELDATA_H__
#define __AMEE_MODELDATA_H__
#pragma once
#include <vector>
#include <cstdint>
#include "../AMEEVertexLayout.hpp"

namespace AMEE {

struct ModelData {
    std::vector<float> Vertices;
    std::vector<uint32_t> Indices;
    VertexLayout Layout;
};

} // namespace AMEE

#endif // __AMEE_MODELDATA_H__
