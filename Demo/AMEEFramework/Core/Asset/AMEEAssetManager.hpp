#ifndef __AMEE_ASSETMANAGER_H__
#define __AMEE_ASSETMANAGER_H__
#pragma once
#include "../AMEESingleton.hpp"
#include "../AMEEBuiltIDs.hpp"
#include "AMEEAssetHandle.hpp"
#include "../../Render/Texture/AMEETexture2D.hpp"
#include "../../Render/Shader/AMEEShaderProgram.hpp"
#include "../../Render/AMEEMesh.hpp"
#include "../../Render/Material/AMEEMaterial.hpp"
#include <string>
#include <vector>
#include <memory>
#include <cstdint>
#include <unordered_map>

namespace AMEE {

class RHI;

class AssetManager : public SingletonT<AssetManager> {
    friend class SingletonT<AssetManager>;
public:
    // Initialize built-in resources
    void InitializeBuiltins(RHI* rhi);

    // Get built-in resource by ID
    ShaderHandle GetBuiltinShader(BuiltID id) const;
    TextureHandle GetBuiltinTexture(BuiltID id) const;
    MaterialHandle GetBuiltinMaterial(BuiltID id) const;

    // Texture
    TextureHandle LoadTexture(RHI* rhi, const std::string& LogicalPath);
    TextureHandle RegisterTexture(std::unique_ptr<Texture2D> InTex, const std::string& Name, uint64_t ID = ID_NULL, bool IsBuiltIn = false);
    Texture2D* GetTexture(TextureHandle Handle) const;
    void UnloadTexture(TextureHandle Handle);

    // Shader
    ShaderHandle LoadShader(RHI* rhi, const std::string& VsPath, const std::string& FsPath, const std::string& Name = "", uint64_t ID = ID_NULL, bool IsBuiltIn = false);
    ShaderHandle RegisterShader(std::unique_ptr<ShaderProgram> InShader, const std::string& Name, uint64_t ID = ID_NULL, bool IsBuiltIn = false);
    ShaderProgram* GetShader(ShaderHandle Handle) const;
    void UnloadShader(ShaderHandle Handle);

    // Mesh
    MeshHandle RegisterMesh(std::unique_ptr<Mesh> InMesh, const std::string& Name, uint64_t ID = ID_NULL, bool IsBuiltIn = false);
    MeshHandle LoadModel(RHI* rhi, const std::string& LogicalPath,
                         std::vector<MaterialHandle>* OutMaterials = nullptr);
    Mesh* GetMesh(MeshHandle Handle) const;
    void UnloadMesh(MeshHandle Handle);

    // Material
    MaterialHandle RegisterMaterial(std::unique_ptr<Material> InMat, uint64_t ID = ID_NULL, bool IsBuiltIn = false);
    Material* GetMaterial(MaterialHandle Handle) const;
    void UnloadMaterial(MaterialHandle Handle);

    // Bulk
    void UnloadAll();

    // Stats
    size_t GetTextureCount() const  { return m_Textures.size(); }
    size_t GetShaderCount() const   { return m_Shaders.size(); }
    size_t GetMeshCount() const     { return m_Meshes.size(); }
    size_t GetMaterialCount() const { return m_Materials.size(); }

private:
    AssetManager() = default;

    // Built-in resource initialization helpers
    void InitBuiltinTextures(RHI* rhi);
    void InitBuiltinShaders(RHI* rhi);
    void InitBuiltinMaterials(RHI* rhi);

    struct TextureEntry {
        std::unique_ptr<Texture2D> Resource;
        std::string Path;
        uint32_t RefCount = 0;
    };

    struct ShaderEntry {
        std::unique_ptr<ShaderProgram> Resource;
        std::string VsPath;
        std::string FsPath;
        uint32_t RefCount = 0;
    };

    struct MeshEntry {
        std::unique_ptr<Mesh> Resource;
        std::string Name;
        uint32_t RefCount = 0;
    };

    struct MaterialEntry {
        std::unique_ptr<Material> Resource;
        std::string Name;
        uint32_t RefCount = 0;
    };

    std::vector<TextureEntry> m_Textures;
    std::vector<ShaderEntry> m_Shaders;
    std::vector<MeshEntry> m_Meshes;
    std::vector<MaterialEntry> m_Materials;

    // Built-in resource handle mappings
    std::unordered_map<uint64_t, ShaderHandle> m_BuiltinShaders;
    std::unordered_map<uint64_t, TextureHandle> m_BuiltinTextures;
    std::unordered_map<uint64_t, MaterialHandle> m_BuiltinMaterials;
};

} // namespace AMEE

#endif // __AMEE_ASSETMANAGER_H__
