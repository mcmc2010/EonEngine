#include "AMEEPrimitiveMesh.hpp"
#include "../../Render/AMEEVertexLayout.hpp"
#include "../Log/AMEELog.hpp"
#include <cmath>

namespace AMEE {

// =============================================================================
// Cube
// =============================================================================
Mesh* PrimitiveMesh::CreateCube(RHI* rhi, float Size)
{
    float H = Size * 0.5f;

    // 6 faces * 4 vertices * 8 floats
    float Vertices[] = {
        // Front face (z+)
        -H, -H,  H,   0,  0,  1,   0, 0,
         H, -H,  H,   0,  0,  1,   1, 0,
         H,  H,  H,   0,  0,  1,   1, 1,
        -H,  H,  H,   0,  0,  1,   0, 1,

        // Back face (z-)
         H, -H, -H,   0,  0, -1,   0, 0,
        -H, -H, -H,   0,  0, -1,   1, 0,
        -H,  H, -H,   0,  0, -1,   1, 1,
         H,  H, -H,   0,  0, -1,   0, 1,

        // Top face (y+)
        -H,  H,  H,   0,  1,  0,   0, 0,
         H,  H,  H,   0,  1,  0,   1, 0,
         H,  H, -H,   0,  1,  0,   1, 1,
        -H,  H, -H,   0,  1,  0,   0, 1,

        // Bottom face (y-)
        -H, -H, -H,   0, -1,  0,   0, 0,
         H, -H, -H,   0, -1,  0,   1, 0,
         H, -H,  H,   0, -1,  0,   1, 1,
        -H, -H,  H,   0, -1,  0,   0, 1,

        // Right face (x+)
         H, -H,  H,   1,  0,  0,   0, 0,
         H, -H, -H,   1,  0,  0,   1, 0,
         H,  H, -H,   1,  0,  0,   1, 1,
         H,  H,  H,   1,  0,  0,   0, 1,

        // Left face (x-)
        -H, -H, -H,  -1,  0,  0,   0, 0,
        -H, -H,  H,  -1,  0,  0,   1, 0,
        -H,  H,  H,  -1,  0,  0,   1, 1,
        -H,  H, -H,  -1,  0,  0,   0, 1,
    };

    uint32_t Indices[] = {
         0,  1,  2,  0,  2,  3,   // Front
         4,  5,  6,  4,  6,  7,   // Back
         8,  9, 10,  8, 10, 11,   // Top
        12, 13, 14, 12, 14, 15,   // Bottom
        16, 17, 18, 16, 18, 19,   // Right
        20, 21, 22, 20, 22, 23,   // Left
    };

    VertexLayout Layout;
    Layout.Add(0, 3, RHIDataType::Float)   // position
          .Add(1, 3, RHIDataType::Float)   // normal
          .Add(2, 2, RHIDataType::Float);  // texcoord

    Mesh* M = new Mesh();
    if (!M->CreateIndexed(rhi, Vertices, 24, Indices, 36, Layout)) {
        AMEE_LOG_ERROR("PrimitiveMesh", "Failed to create cube");
        delete M;
        return nullptr;
    }

    AMEE_LOG_INFO("PrimitiveMesh", "Created cube (size=%.2f)", Size);
    return M;
}

// =============================================================================
// Sphere
// =============================================================================
Mesh* PrimitiveMesh::CreateSphere(RHI* rhi, float Radius, int Segments)
{
    std::vector<float> Vertices;
    std::vector<uint32_t> Indices;

    GenerateSphereVertices(Vertices, Indices, Radius, Segments);

    VertexLayout Layout;
    Layout.Add(0, 3, RHIDataType::Float)   // position
          .Add(1, 3, RHIDataType::Float)   // normal
          .Add(2, 2, RHIDataType::Float);  // texcoord

    uint32_t VertexCount = (uint32_t)(Vertices.size() / VERTEX_SIZE);
    uint32_t IndexCount = (uint32_t)Indices.size();

    Mesh* M = new Mesh();
    if (!M->CreateIndexed(rhi, Vertices.data(), VertexCount, Indices.data(), IndexCount, Layout)) {
        AMEE_LOG_ERROR("PrimitiveMesh", "Failed to create sphere");
        delete M;
        return nullptr;
    }

    AMEE_LOG_INFO("PrimitiveMesh", "Created sphere (radius=%.2f, segments=%d)", Radius, Segments);
    return M;
}

void PrimitiveMesh::GenerateSphereVertices(std::vector<float>& Vertices, std::vector<uint32_t>& Indices,
                                            float Radius, int Segments)
{
    int Rings = Segments;
    int Sectors = Segments;

    for (int R = 0; R <= Rings; R++) {
        float Phi = (float)M_PI * (float)R / (float)Rings;
        float SinPhi = std::sin(Phi);
        float CosPhi = std::cos(Phi);

        for (int S = 0; S <= Sectors; S++) {
            float Theta = 2.0f * (float)M_PI * (float)S / (float)Sectors;
            float SinTheta = std::sin(Theta);
            float CosTheta = std::cos(Theta);

            float X = CosTheta * SinPhi;
            float Y = CosPhi;
            float Z = SinTheta * SinPhi;

            // Position
            Vertices.push_back(X * Radius);
            Vertices.push_back(Y * Radius);
            Vertices.push_back(Z * Radius);

            // Normal
            Vertices.push_back(X);
            Vertices.push_back(Y);
            Vertices.push_back(Z);

            // Texcoord
            Vertices.push_back((float)S / (float)Sectors);
            Vertices.push_back((float)R / (float)Rings);
        }
    }

    // Generate indices
    for (int R = 0; R < Rings; R++) {
        for (int S = 0; S < Sectors; S++) {
            int A = R * (Sectors + 1) + S;
            int B = A + Sectors + 1;

            Indices.push_back(A);
            Indices.push_back(B);
            Indices.push_back(A + 1);

            Indices.push_back(A + 1);
            Indices.push_back(B);
            Indices.push_back(B + 1);
        }
    }
}

// =============================================================================
// Plane
// =============================================================================
Mesh* PrimitiveMesh::CreatePlane(RHI* rhi, float Width, float Height)
{
    float HW = Width * 0.5f;
    float HH = Height * 0.5f;

    float Vertices[] = {
        // position           normal        texcoord
        -HW, 0, -HH,   0, 1, 0,   0, 0,
         HW, 0, -HH,   0, 1, 0,   1, 0,
         HW, 0,  HH,   0, 1, 0,   1, 1,
        -HW, 0,  HH,   0, 1, 0,   0, 1,
    };

    uint32_t Indices[] = {
        0, 1, 2,
        0, 2, 3,
    };

    VertexLayout Layout;
    Layout.Add(0, 3, RHIDataType::Float)
          .Add(1, 3, RHIDataType::Float)
          .Add(2, 2, RHIDataType::Float);

    Mesh* M = new Mesh();
    if (!M->CreateIndexed(rhi, Vertices, 4, Indices, 6, Layout)) {
        AMEE_LOG_ERROR("PrimitiveMesh", "Failed to create plane");
        delete M;
        return nullptr;
    }

    AMEE_LOG_INFO("PrimitiveMesh", "Created plane (%.2fx%.2f)", Width, Height);
    return M;
}

// =============================================================================
// Cylinder
// =============================================================================
Mesh* PrimitiveMesh::CreateCylinder(RHI* rhi, float Radius, float Height, int Segments)
{
    std::vector<float> Vertices;
    std::vector<uint32_t> Indices;

    float HalfH = Height * 0.5f;
    int Sides = Segments;

    // Side vertices (top and bottom rings)
    for (int I = 0; I <= Sides; I++) {
        float Angle = 2.0f * (float)M_PI * (float)I / (float)Sides;
        float X = std::cos(Angle);
        float Z = std::sin(Angle);
        float U = (float)I / (float)Sides;

        // Bottom vertex
        Vertices.push_back(X * Radius);  // position
        Vertices.push_back(-HalfH);
        Vertices.push_back(Z * Radius);
        Vertices.push_back(X);           // normal
        Vertices.push_back(0);
        Vertices.push_back(Z);
        Vertices.push_back(U);           // texcoord
        Vertices.push_back(0);

        // Top vertex
        Vertices.push_back(X * Radius);
        Vertices.push_back(HalfH);
        Vertices.push_back(Z * Radius);
        Vertices.push_back(X);
        Vertices.push_back(0);
        Vertices.push_back(Z);
        Vertices.push_back(U);
        Vertices.push_back(1);
    }

    // Side indices
    for (int I = 0; I < Sides; I++) {
        int A = I * 2;
        int B = A + 1;
        int C = A + 2;
        int D = A + 3;

        Indices.push_back(A);
        Indices.push_back(C);
        Indices.push_back(B);

        Indices.push_back(B);
        Indices.push_back(C);
        Indices.push_back(D);
    }

    // Top cap
    int TopCenterIdx = (int)(Vertices.size() / VERTEX_SIZE);
    Vertices.push_back(0);      // position
    Vertices.push_back(HalfH);
    Vertices.push_back(0);
    Vertices.push_back(0);      // normal
    Vertices.push_back(1);
    Vertices.push_back(0);
    Vertices.push_back(0.5f);   // texcoord
    Vertices.push_back(0.5f);

    for (int I = 0; I <= Sides; I++) {
        float Angle = 2.0f * (float)M_PI * (float)I / (float)Sides;
        float X = std::cos(Angle);
        float Z = std::sin(Angle);

        int Idx = (int)(Vertices.size() / VERTEX_SIZE);
        Vertices.push_back(X * Radius);
        Vertices.push_back(HalfH);
        Vertices.push_back(Z * Radius);
        Vertices.push_back(0);
        Vertices.push_back(1);
        Vertices.push_back(0);
        Vertices.push_back(0.5f + X * 0.5f);
        Vertices.push_back(0.5f + Z * 0.5f);

        if (I > 0) {
            Indices.push_back(TopCenterIdx);
            Indices.push_back(Idx - 1);
            Indices.push_back(Idx);
        }
    }

    // Bottom cap
    int BotCenterIdx = (int)(Vertices.size() / VERTEX_SIZE);
    Vertices.push_back(0);
    Vertices.push_back(-HalfH);
    Vertices.push_back(0);
    Vertices.push_back(0);
    Vertices.push_back(-1);
    Vertices.push_back(0);
    Vertices.push_back(0.5f);
    Vertices.push_back(0.5f);

    for (int I = 0; I <= Sides; I++) {
        float Angle = 2.0f * (float)M_PI * (float)I / (float)Sides;
        float X = std::cos(Angle);
        float Z = std::sin(Angle);

        int Idx = (int)(Vertices.size() / VERTEX_SIZE);
        Vertices.push_back(X * Radius);
        Vertices.push_back(-HalfH);
        Vertices.push_back(Z * Radius);
        Vertices.push_back(0);
        Vertices.push_back(-1);
        Vertices.push_back(0);
        Vertices.push_back(0.5f + X * 0.5f);
        Vertices.push_back(0.5f + Z * 0.5f);

        if (I > 0) {
            Indices.push_back(BotCenterIdx);
            Indices.push_back(Idx);
            Indices.push_back(Idx - 1);
        }
    }

    VertexLayout Layout;
    Layout.Add(0, 3, RHIDataType::Float)
          .Add(1, 3, RHIDataType::Float)
          .Add(2, 2, RHIDataType::Float);

    uint32_t VertexCount = (uint32_t)(Vertices.size() / VERTEX_SIZE);
    uint32_t IndexCount = (uint32_t)Indices.size();

    Mesh* M = new Mesh();
    if (!M->CreateIndexed(rhi, Vertices.data(), VertexCount, Indices.data(), IndexCount, Layout)) {
        AMEE_LOG_ERROR("PrimitiveMesh", "Failed to create cylinder");
        delete M;
        return nullptr;
    }

    AMEE_LOG_INFO("PrimitiveMesh", "Created cylinder (r=%.2f, h=%.2f)", Radius, Height);
    return M;
}

// =============================================================================
// Capsule
// =============================================================================
Mesh* PrimitiveMesh::CreateCapsule(RHI* rhi, float Radius, float Height, int Segments)
{
    std::vector<float> Vertices;
    std::vector<uint32_t> Indices;

    int Rings = Segments / 2;
    int Sides = Segments;
    float HalfH = Height * 0.5f;

    // Top hemisphere
    for (int R = 0; R <= Rings; R++) {
        float Phi = (float)M_PI * 0.5f * (float)R / (float)Rings;
        float SinPhi = std::sin(Phi);
        float CosPhi = std::cos(Phi);

        for (int S = 0; S <= Sides; S++) {
            float Theta = 2.0f * (float)M_PI * (float)S / (float)Sides;
            float SinTheta = std::sin(Theta);
            float CosTheta = std::cos(Theta);

            float X = CosTheta * CosPhi;
            float Y = SinPhi;
            float Z = SinTheta * CosPhi;

            Vertices.push_back(X * Radius);
            Vertices.push_back(Y * Radius + HalfH);
            Vertices.push_back(Z * Radius);

            Vertices.push_back(X);
            Vertices.push_back(Y);
            Vertices.push_back(Z);

            Vertices.push_back((float)S / (float)Sides);
            Vertices.push_back((float)R / (float)Rings * 0.25f + 0.75f);
        }
    }

    // Bottom hemisphere
    for (int R = 0; R <= Rings; R++) {
        float Phi = (float)M_PI * 0.5f * (float)R / (float)Rings;
        float SinPhi = std::sin(Phi);
        float CosPhi = std::cos(Phi);

        for (int S = 0; S <= Sides; S++) {
            float Theta = 2.0f * (float)M_PI * (float)S / (float)Sides;
            float SinTheta = std::sin(Theta);
            float CosTheta = std::cos(Theta);

            float X = CosTheta * CosPhi;
            float Y = -SinPhi;
            float Z = SinTheta * CosPhi;

            Vertices.push_back(X * Radius);
            Vertices.push_back(Y * Radius - HalfH);
            Vertices.push_back(Z * Radius);

            Vertices.push_back(X);
            Vertices.push_back(Y);
            Vertices.push_back(Z);

            Vertices.push_back((float)S / (float)Sides);
            Vertices.push_back(0.25f - (float)R / (float)Rings * 0.25f);
        }
    }

    // Body (connecting cylinder)
    int BodyStart = (int)(Vertices.size() / VERTEX_SIZE);
    for (int S = 0; S <= Sides; S++) {
        float Theta = 2.0f * (float)M_PI * (float)S / (float)Sides;
        float X = std::cos(Theta);
        float Z = std::sin(Theta);
        float U = (float)S / (float)Sides;

        // Bottom
        Vertices.push_back(X * Radius);
        Vertices.push_back(-HalfH);
        Vertices.push_back(Z * Radius);
        Vertices.push_back(X);
        Vertices.push_back(0);
        Vertices.push_back(Z);
        Vertices.push_back(U);
        Vertices.push_back(0.25f);

        // Top
        Vertices.push_back(X * Radius);
        Vertices.push_back(HalfH);
        Vertices.push_back(Z * Radius);
        Vertices.push_back(X);
        Vertices.push_back(0);
        Vertices.push_back(Z);
        Vertices.push_back(U);
        Vertices.push_back(0.75f);
    }

    // Body indices
    for (int S = 0; S < Sides; S++) {
        int A = BodyStart + S * 2;
        int B = A + 1;
        int C = A + 2;
        int D = A + 3;

        Indices.push_back(A);
        Indices.push_back(C);
        Indices.push_back(B);

        Indices.push_back(B);
        Indices.push_back(C);
        Indices.push_back(D);
    }

    // Top hemisphere indices
    for (int R = 0; R < Rings; R++) {
        for (int S = 0; S < Sides; S++) {
            int A = R * (Sides + 1) + S;
            int B = A + Sides + 1;

            Indices.push_back(A);
            Indices.push_back(A + 1);
            Indices.push_back(B);

            Indices.push_back(A + 1);
            Indices.push_back(B + 1);
            Indices.push_back(B);
        }
    }

    // Bottom hemisphere indices
    int BottomStart = (Rings + 1) * (Sides + 1);
    for (int R = 0; R < Rings; R++) {
        for (int S = 0; S < Sides; S++) {
            int A = BottomStart + R * (Sides + 1) + S;
            int B = A + Sides + 1;

            Indices.push_back(A);
            Indices.push_back(B);
            Indices.push_back(A + 1);

            Indices.push_back(A + 1);
            Indices.push_back(B);
            Indices.push_back(B + 1);
        }
    }

    VertexLayout Layout;
    Layout.Add(0, 3, RHIDataType::Float)
          .Add(1, 3, RHIDataType::Float)
          .Add(2, 2, RHIDataType::Float);

    uint32_t VertexCount = (uint32_t)(Vertices.size() / VERTEX_SIZE);
    uint32_t IndexCount = (uint32_t)Indices.size();

    Mesh* M = new Mesh();
    if (!M->CreateIndexed(rhi, Vertices.data(), VertexCount, Indices.data(), IndexCount, Layout)) {
        AMEE_LOG_ERROR("PrimitiveMesh", "Failed to create capsule");
        delete M;
        return nullptr;
    }

    AMEE_LOG_INFO("PrimitiveMesh", "Created capsule (r=%.2f, h=%.2f)", Radius, Height);
    return M;
}

} // namespace AMEE