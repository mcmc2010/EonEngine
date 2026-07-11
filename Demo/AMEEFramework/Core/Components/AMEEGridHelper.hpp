#ifndef __AMEE_GRIDHELPER_H__
#define __AMEE_GRIDHELPER_H__
#pragma once
#include "AMEEComponent.hpp"
#include "../Math/AMEEMath.hpp"
#include <cstdint>

namespace AMEE {

class RHI;

class GridHelper : public Component {
public:
    GridHelper() = default;

    bool Create(RHI* rhi, float Size = 10.0f, int Divisions = 10);
    void Draw(RHI* rhi, const Mat4& VP);

private:
    uint32_t m_VAO = 0;
    uint32_t m_VBO = 0;
    uint32_t m_LineCount = 0;
};

} // namespace AMEE

#endif // __AMEE_GRIDHELPER_H__
