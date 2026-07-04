#ifndef __AMEE_PRIMITIVEMESH_H__
#define __AMEE_PRIMITIVEMESH_H__
#pragma once
#include "../../Render/AMEEMesh.hpp"

namespace AMEE {

class PrimitiveMesh {
public:
    // Each vertex: position(3) + normal(3) + texcoord(2) = 8 floats
    static constexpr int VERTEX_SIZE = 8;

    // Basic primitives
    static Mesh* CreateCube(RHI* rhi, float Size = 1.0f);
    static Mesh* CreateSphere(RHI* rhi, float Radius = 1.0f, int Segments = 32);
    static Mesh* CreatePlane(RHI* rhi, float Width = 1.0f, float Height = 1.0f);
    static Mesh* CreateCylinder(RHI* rhi, float Radius = 1.0f, float Height = 1.0f, int Segments = 32);
    static Mesh* CreateCapsule(RHI* rhi, float Radius = 0.5f, float Height = 1.0f, int Segments = 32);

private:
    // Helper to generate UV sphere vertices
    static void GenerateSphereVertices(std::vector<float>& Vertices, std::vector<uint32_t>& Indices,
                                        float Radius, int Segments);
};

} // namespace AMEE

#endif // __AMEE_PRIMITIVEMESH_H__