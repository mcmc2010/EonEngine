#include "AMEEObjLoader.hpp"
#include "../../Core/Log/AMEELog.hpp"
#include "../../Core/Math/AMEEMath.hpp"
#include <sstream>
#include <cstring>
#include <unordered_map>
#include <cmath>

namespace AMEE {

struct ObjVertexKey {
    int PosIdx, TexIdx, NormIdx;
    bool operator==(const ObjVertexKey& O) const {
        return PosIdx == O.PosIdx && TexIdx == O.TexIdx && NormIdx == O.NormIdx;
    }
};

struct ObjVertexKeyHash {
    size_t operator()(const ObjVertexKey& K) const {
        return ((size_t)K.PosIdx << 20) ^ ((size_t)K.TexIdx << 10) ^ (size_t)K.NormIdx;
    }
};

ModelData ObjLoader::Load(const std::string& Source)
{
    ModelData Result;

    std::vector<Vec3> Positions;
    std::vector<Vec2> TexCoords;
    std::vector<Vec3> Normals;

    struct FaceVert { int P, T, N; };
    struct FaceGroup {
        std::string Material;
        std::vector<std::vector<FaceVert>> Faces;
    };

    std::vector<FaceGroup> Groups;
    std::string CurrentMaterial = "default";
    bool HasTex = false;
    bool HasNorm = false;

    std::istringstream Stream(Source);
    std::string Line;

    while (std::getline(Stream, Line)) {
        if (Line.empty() || Line[0] == '#') continue;

        std::istringstream LS(Line);
        std::string Token;
        LS >> Token;

        if (Token == "v") {
            float X, Y, Z;
            LS >> X >> Y >> Z;
            Positions.push_back({X, Y, Z});
        } else if (Token == "vt") {
            float U, V;
            LS >> U >> V;
            TexCoords.push_back({U, V});
            HasTex = true;
        } else if (Token == "vn") {
            float X, Y, Z;
            LS >> X >> Y >> Z;
            Normals.push_back({X, Y, Z});
            HasNorm = true;
        } else if (Token == "usemtl") {
            LS >> CurrentMaterial;
        } else if (Token == "f") {
            std::vector<FaceVert> Face;
            std::string Part;
            while (LS >> Part) {
                FaceVert FV = {-1, -1, -1};
                char* End;
                FV.P = (int)std::strtol(Part.c_str(), &End, 10) - 1;
                if (*End == '/') {
                    char* End2;
                    FV.T = (int)std::strtol(End + 1, &End2, 10) - 1;
                    if (*End2 == '/') {
                        FV.N = (int)std::strtol(End2 + 1, nullptr, 10) - 1;
                    }
                }
                Face.push_back(FV);
            }

            if (Groups.empty() || Groups.back().Material != CurrentMaterial) {
                Groups.push_back({CurrentMaterial, {}});
            }

            auto& G = Groups.back();
            if (Face.size() == 3) {
                G.Faces.push_back(Face);
            } else if (Face.size() == 4) {
                G.Faces.push_back({Face[0], Face[1], Face[2]});
                G.Faces.push_back({Face[0], Face[2], Face[3]});
            } else if (Face.size() > 4) {
                for (size_t I = 1; I < Face.size() - 1; I++) {
                    G.Faces.push_back({Face[0], Face[I], Face[I + 1]});
                }
            }
        }
    }

    if (Groups.empty()) {
        AMEE_LOG_ERROR("ObjLoader", "No faces found");
        return Result;
    }

    if (!HasNorm) {
        Normals.resize(Positions.size(), {0, 0, 0});
        for (auto& G : Groups) {
            for (auto& Face : G.Faces) {
                if (Face.size() >= 3) {
                    Vec3 A = Positions[Face[0].P];
                    Vec3 B = Positions[Face[1].P];
                    Vec3 C = Positions[Face[2].P];
                    Vec3 N = Vec3::Cross(B - A, C - A);
                    for (auto& FV : Face) {
                        if (FV.P >= 0 && FV.P < (int)Normals.size()) {
                            Normals[FV.P] = Normals[FV.P] + N;
                        }
                    }
                }
            }
        }
        for (auto& N : Normals) {
            float Len = N.Length();
            if (Len > 0) { N = N / Len; }
        }
        HasNorm = true;
    }

    if (!HasTex) {
        TexCoords.push_back({0, 0});
    }

    std::unordered_map<ObjVertexKey, uint32_t, ObjVertexKeyHash> IndexMap;
    uint32_t NextIndex = 0;

    for (auto& G : Groups) {
        SubMesh Sub;
        Sub.MaterialName = G.Material;
        Sub.IndexStart = (uint32_t)Result.Indices.size();

        for (auto& Face : G.Faces) {
            for (auto& FV : Face) {
                int TI = HasTex && FV.T >= 0 ? FV.T : 0;
                int NI = HasNorm && FV.N >= 0 ? FV.N : (FV.P >= 0 && FV.P < (int)Normals.size() ? FV.P : 0);
                if (FV.P < 0 || FV.P >= (int)Positions.size()) continue;

                ObjVertexKey Key = {FV.P, TI, NI};
                auto It = IndexMap.find(Key);
                if (It != IndexMap.end()) {
                    Result.Indices.push_back(It->second);
                    Sub.IndexCount++;
                    continue;
                }

                uint32_t Idx = NextIndex++;
                IndexMap[Key] = Idx;
                Result.Indices.push_back(Idx);
                Sub.IndexCount++;

                Vec3 P = Positions[FV.P];
                Vec3 N = (NI >= 0 && NI < (int)Normals.size()) ? Normals[NI] : Vec3(0, 1, 0);
                Vec2 T = (TI >= 0 && TI < (int)TexCoords.size()) ? TexCoords[TI] : Vec2(0, 0);

                Result.Vertices.insert(Result.Vertices.end(), {P.x, P.y, P.z, N.x, N.y, N.z, T.x, T.y});
            }
        }

        Result.SubMeshes.push_back(Sub);
        Result.MaterialNames.push_back(G.Material);
    }

    Result.Layout.Add(0, 3, RHIDataType::Float)
                 .Add(1, 3, RHIDataType::Float)
                 .Add(2, 2, RHIDataType::Float);

    AMEE_LOG_INFO("ObjLoader", "Loaded %zu vertices, %zu indices, %zu submeshes",
                  Result.Vertices.size() / 8, Result.Indices.size(), Result.SubMeshes.size());
    return Result;
}

std::string ObjLoader::ExtractMtlLib(const std::string& Source)
{
    std::istringstream Stream(Source);
    std::string Line;
    while (std::getline(Stream, Line)) {
        std::istringstream LS(Line);
        std::string Token;
        LS >> Token;
        if (Token == "mtllib") {
            std::string Path;
            LS >> Path;
            return Path;
        }
    }
    return {};
}

std::vector<std::string> ObjLoader::ExtractMaterialNames(const std::string& Source)
{
    std::vector<std::string> Names;
    std::istringstream Stream(Source);
    std::string Line;
    std::string Last;
    while (std::getline(Stream, Line)) {
        std::istringstream LS(Line);
        std::string Token;
        LS >> Token;
        if (Token == "usemtl") {
            std::string Name;
            LS >> Name;
            if (Name != Last) {
                Names.push_back(Name);
                Last = Name;
            }
        }
    }
    return Names;
}

} // namespace AMEE
