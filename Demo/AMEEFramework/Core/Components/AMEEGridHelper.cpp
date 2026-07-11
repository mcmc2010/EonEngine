#include "AMEEGridHelper.hpp"
#include "../../Render/AMEERHI.hpp"
#include "../Log/AMEELog.hpp"
#include <vector>

namespace AMEE {

bool GridHelper::Create(RHI* rhi, float Size, int Divisions)
{
    if (!rhi || Divisions <= 0) return false;

    float Half = Size * 0.5f;
    float Step = Size / (float)Divisions;

    std::vector<float> Vertices;

    auto AddLine = [&](float X1, float Z1, float X2, float Z2,
                       float R, float G, float B) {
        // Each vertex: position(3)
        Vertices.insert(Vertices.end(), {X1, 0, Z1, X2, 0, Z2});
    };

    float Gray[] = {0.4f, 0.4f, 0.4f};
    float Red[]  = {1.0f, 0.0f, 0.0f};
    float Blue[] = {0.0f, 0.0f, 1.0f};

    for (int I = 0; I <= Divisions; I++) {
        float P = -Half + I * Step;
        float* C = (I == Divisions / 2) ? Red : Gray;
        AddLine(P, -Half, P, Half, C[0], C[1], C[2]);  // X lines
        C = (I == Divisions / 2) ? Blue : Gray;
        AddLine(-Half, P, Half, P, C[0], C[1], C[2]);  // Z lines
    }

    m_LineCount = (uint32_t)(Vertices.size() / 3) / 2;

    m_VBO = rhi->createVertexBuffer(Vertices.data(), (uint32_t)(Vertices.size() * sizeof(float)));
    if (m_VBO == 0) {
        AMEE_LOG_ERROR("GridHelper", "Failed to create VBO");
        return false;
    }

    m_VAO = rhi->createVertexArray();
    if (m_VAO == 0) {
        AMEE_LOG_ERROR("GridHelper", "Failed to create VAO");
        return false;
    }

    rhi->bindVertexArray(m_VAO);
    rhi->bindVertexBuffer(m_VBO);
    rhi->enableVertexAttribArray(0);
    rhi->vertexAttribPointer(0, 3, RHIDataType::Float, false, 3 * sizeof(float), nullptr);
    rhi->bindVertexArray(0);
    rhi->bindVertexBuffer(0);

    AMEE_LOG_INFO("GridHelper", "Created grid %.0fx%.0f (%d divisions, %u lines)",
                  Size, Size, Divisions, m_LineCount);
    return true;
}

void GridHelper::Draw(RHI* rhi, const Mat4& VP)
{
    if (!rhi || m_VAO == 0 || m_LineCount == 0) return;

    // Simple line shader — just use VP, white lines with w=1.0
    // MVP is set by caller
    rhi->bindVertexArray(m_VAO);
    rhi->drawArrays(RHIPrimitive::Lines, m_LineCount * 2, 0);
    rhi->bindVertexArray(0);
}

} // namespace AMEE
