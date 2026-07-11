#include "AMEEAssetManager.hpp"
#include "AMEEFileSystem.hpp"
#include "../Log/AMEELog.hpp"
#include "../../Render/AMEERHI.hpp"
#include "../../Render/Texture/AMEETexture2D.hpp"
#include "../../Render/Shader/AMEEShaderProgram.hpp"
#include "../../Render/AMEEMesh.hpp"
#include "../../Render/Model/AMEEObjLoader.hpp"
#include "../../Render/Material/AMEEMaterial.hpp"
#include "../../Render/Material/AMEEStandardMaterial.hpp"
#include "../../Render/Material/AMEEStandardMaterialImporter.hpp"

namespace AMEE {

AssetManager& AssetManager::Instance()
{
    static AssetManager mgr;
    return mgr;
}

// ─── Texture ──────────────────────────────────────────────────────────────────

TextureHandle AssetManager::LoadTexture(RHI* rhi, const std::string& LogicalPath)
{
    if (!rhi) return {};

    for (size_t i = 0; i < m_Textures.size(); ++i) {
        if (m_Textures[i].Path == LogicalPath && m_Textures[i].Resource) {
            m_Textures[i].RefCount++;
            return {static_cast<uint32_t>(i)};
        }
    }

    std::string PhysicalPath = FileSystem::Instance().ResolvePath(LogicalPath);
    if (PhysicalPath.empty()) {
        AMEE_LOG_ERROR("AssetManager", "Cannot resolve path: %s", LogicalPath.c_str());
        return {};
    }

    auto Tex = std::make_unique<Texture2D>();
    if (!Tex->Load(rhi, PhysicalPath)) {
        AMEE_LOG_ERROR("AssetManager", "Failed to load texture: %s", LogicalPath.c_str());
        return {};
    }

    return RegisterTexture(std::move(Tex), LogicalPath);
}

TextureHandle AssetManager::RegisterTexture(std::unique_ptr<Texture2D> InTex, const std::string& Name)
{
    if (!InTex) return {};
    uint32_t Idx = static_cast<uint32_t>(m_Textures.size());
    m_Textures.push_back({std::move(InTex), Name, 1});
    AMEE_LOG_INFO("AssetManager", "Registered texture [%u]: %s", Idx, Name.c_str());
    return {Idx};
}

Texture2D* AssetManager::GetTexture(TextureHandle Handle) const
{
    if (!Handle.IsValid() || Handle.Index >= m_Textures.size()) return nullptr;
    return m_Textures[Handle.Index].Resource.get();
}

void AssetManager::UnloadTexture(TextureHandle Handle)
{
    if (!Handle.IsValid() || Handle.Index >= m_Textures.size()) return;
    auto& Entry = m_Textures[Handle.Index];
    if (Entry.RefCount == 0) return;

    Entry.RefCount--;
    if (Entry.RefCount == 0) {
        Entry.Resource.reset();
        AMEE_LOG_INFO("AssetManager", "Unloaded texture [%u]: %s", Handle.Index, Entry.Path.c_str());
    }
}

// ─── Shader ────────────────────────────────────────────────────────────────────

ShaderHandle AssetManager::LoadShader(RHI* rhi, const std::string& VsPath, const std::string& FsPath)
{
    if (!rhi) return {};

    for (size_t i = 0; i < m_Shaders.size(); ++i) {
        if (m_Shaders[i].VsPath == VsPath && m_Shaders[i].FsPath == FsPath && m_Shaders[i].Resource) {
            m_Shaders[i].RefCount++;
            return {static_cast<uint32_t>(i)};
        }
    }

    std::string VsSource = FileSystem::Instance().ReadText(VsPath);
    std::string FsSource = FileSystem::Instance().ReadText(FsPath);
    if (VsSource.empty() || FsSource.empty()) {
        AMEE_LOG_ERROR("AssetManager", "Failed to read shader sources: %s / %s",
                       VsPath.c_str(), FsPath.c_str());
        return {};
    }

    auto Shader = rhi->CreateShaderProgram();
    bool CompileOk = true;

    Shader->compileFromSource(ShaderType::Vertex, VsSource.c_str(), [&](const ShaderCompileError& Err) {
        AMEE_LOG_ERROR("AssetManager", "Vertex compile error: %s", Err.message.c_str());
        CompileOk = false;
    });

    Shader->compileFromSource(ShaderType::Fragment, FsSource.c_str(), [&](const ShaderCompileError& Err) {
        AMEE_LOG_ERROR("AssetManager", "Fragment compile error: %s", Err.message.c_str());
        CompileOk = false;
    });

    if (!CompileOk) {
        AMEE_LOG_ERROR("AssetManager", "Shader compilation failed");
        return {};
    }

    if (!Shader->link([&](const std::string& Err) {
        AMEE_LOG_ERROR("AssetManager", "Shader link error: %s", Err.c_str());
    })) {
        return {};
    }

    return RegisterShader(std::move(Shader), VsPath + " + " + FsPath);
}

ShaderHandle AssetManager::RegisterShader(std::unique_ptr<ShaderProgram> InShader, const std::string& Name)
{
    if (!InShader) return {};
    uint32_t Idx = static_cast<uint32_t>(m_Shaders.size());
    m_Shaders.push_back({std::move(InShader), Name, "", 1});
    AMEE_LOG_INFO("AssetManager", "Registered shader [%u]: %s", Idx, Name.c_str());
    return {Idx};
}

ShaderProgram* AssetManager::GetShader(ShaderHandle Handle) const
{
    if (!Handle.IsValid() || Handle.Index >= m_Shaders.size()) return nullptr;
    return m_Shaders[Handle.Index].Resource.get();
}

void AssetManager::UnloadShader(ShaderHandle Handle)
{
    if (!Handle.IsValid() || Handle.Index >= m_Shaders.size()) return;
    auto& Entry = m_Shaders[Handle.Index];
    if (Entry.RefCount == 0) return;

    Entry.RefCount--;
    if (Entry.RefCount == 0) {
        Entry.Resource.reset();
        AMEE_LOG_INFO("AssetManager", "Unloaded shader [%u]", Handle.Index);
    }
}

// ─── Mesh ──────────────────────────────────────────────────────────────────────

MeshHandle AssetManager::RegisterMesh(std::unique_ptr<Mesh> InMesh, const std::string& Name)
{
    if (!InMesh) return {};
    uint32_t Idx = static_cast<uint32_t>(m_Meshes.size());
    m_Meshes.push_back({std::move(InMesh), Name, 1});
    AMEE_LOG_INFO("AssetManager", "Registered mesh [%u]: %s", Idx, Name.c_str());
    return {Idx};
}

MeshHandle AssetManager::LoadModel(RHI* rhi, const std::string& LogicalPath,
                                   std::vector<MaterialHandle>* OutMaterials)
{
    if (!rhi) return {};

    std::string Source = FileSystem::Instance().ReadText(LogicalPath);
    if (Source.empty()) {
        AMEE_LOG_ERROR("AssetManager", "Failed to read model: %s", LogicalPath.c_str());
        return {};
    }

    ModelData Data = ObjLoader::Load(Source);
    if (Data.Vertices.empty()) {
        AMEE_LOG_ERROR("AssetManager", "Failed to parse model: %s", LogicalPath.c_str());
        return {};
    }

    auto MeshObj = std::make_unique<Mesh>();
    if (!MeshObj->CreateIndexed(rhi, Data.Vertices.data(),
                                 (uint32_t)(Data.Vertices.size() / 8),
                                 Data.Indices.data(), (uint32_t)Data.Indices.size(),
                                 Data.Layout)) {
        AMEE_LOG_ERROR("AssetManager", "Failed to create mesh from: %s", LogicalPath.c_str());
        return {};
    }

    MeshObj->SetSubMeshes(Data.SubMeshes);

    std::string MtlName = ObjLoader::ExtractMtlLib(Source);
    if (!MtlName.empty()) {
        std::string ObjDir = LogicalPath.substr(0, LogicalPath.find_last_of('/') + 1);
        std::string MtlPath = ObjDir + MtlName;
        std::string MtlSource = FileSystem::Instance().ReadText(MtlPath);

        if (!MtlSource.empty()) {
            auto Imported = AMEEStandardMaterialImporter::Parse(MtlSource);
            AMEE_LOG_INFO("AssetManager", "Model '%s' has %zu materials (%zu submeshes):",
                          LogicalPath.c_str(), Imported.size(), Data.SubMeshes.size());

    std::unordered_map<std::string, MaterialHandle> MatMap;
    for (auto& Imp : Imported) {
        if (!Imp.DiffuseTexturePath.empty()) {
            std::string TexPath = ObjDir + Imp.DiffuseTexturePath;
            TextureHandle Tex = LoadTexture(rhi, TexPath);
            if (Tex.IsValid()) Imp.Material->SetAlbedoMap(Tex);
        }
        MaterialHandle H = RegisterMaterial(std::move(Imp.Material));
        MatMap[Imp.Name] = H;
    }

    for (auto& Sub : Data.SubMeshes) {
        auto It = MatMap.find(Sub.MaterialName);
        if (It != MatMap.end() && OutMaterials) {
            OutMaterials->push_back(It->second);
        }
        AMEE_LOG_INFO("AssetManager", "  [%s] → %s",
                      Sub.MaterialName.c_str(),
                      It != MatMap.end() ? "✓" : "✗ (no match)");
    }
        } else {
            AMEE_LOG_WARN("AssetManager", "MTL file not found: %s", MtlPath.c_str());
        }
    }

    return RegisterMesh(std::move(MeshObj), LogicalPath);
}

Mesh* AssetManager::GetMesh(MeshHandle Handle) const
{
    if (!Handle.IsValid() || Handle.Index >= m_Meshes.size()) return nullptr;
    return m_Meshes[Handle.Index].Resource.get();
}

void AssetManager::UnloadMesh(MeshHandle Handle)
{
    if (!Handle.IsValid() || Handle.Index >= m_Meshes.size()) return;
    auto& Entry = m_Meshes[Handle.Index];
    if (Entry.RefCount == 0) return;
    Entry.RefCount--;
    if (Entry.RefCount == 0) {
        Entry.Resource.reset();
        AMEE_LOG_INFO("AssetManager", "Unloaded mesh [%u]: %s", Handle.Index, Entry.Name.c_str());
    }
}

// ─── Material ─────────────────────────────────────────────────────────────────

MaterialHandle AssetManager::RegisterMaterial(std::unique_ptr<Material> InMat)
{
    if (!InMat) return {};
    uint32_t Idx = static_cast<uint32_t>(m_Materials.size());
    m_Materials.push_back({std::move(InMat), "", 1});
    AMEE_LOG_INFO("AssetManager", "Registered material [%u]: %s", Idx, m_Materials.back().Resource->GetName().c_str());
    return {Idx};
}

Material* AssetManager::GetMaterial(MaterialHandle Handle) const
{
    if (!Handle.IsValid() || Handle.Index >= m_Materials.size()) return nullptr;
    return m_Materials[Handle.Index].Resource.get();
}

void AssetManager::UnloadMaterial(MaterialHandle Handle)
{
    if (!Handle.IsValid() || Handle.Index >= m_Materials.size()) return;
    auto& Entry = m_Materials[Handle.Index];
    if (Entry.RefCount == 0) return;
    Entry.RefCount--;
    if (Entry.RefCount == 0) {
        Entry.Resource.reset();
        AMEE_LOG_INFO("AssetManager", "Unloaded material [%u]", Handle.Index);
    }
}

// ─── Bulk ──────────────────────────────────────────────────────────────────────

void AssetManager::UnloadAll()
{
    m_Textures.clear();
    m_Shaders.clear();
    m_Meshes.clear();
    m_Materials.clear();
    AMEE_LOG_INFO("AssetManager", "All assets unloaded");
}

} // namespace AMEE
