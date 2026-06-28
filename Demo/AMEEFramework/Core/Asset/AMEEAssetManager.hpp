#ifndef __AMEE_ASSETMANAGER_H__
#define __AMEE_ASSETMANAGER_H__
#pragma once
#include "AMEEAssetHandle.hpp"
#include <string>
#include <vector>
#include <memory>
#include <cstdint>

namespace AMEE {

class RHI;
class Texture2D;
class ShaderProgram;

class AssetManager {
public:
    static AssetManager& Instance();

    AssetManager(const AssetManager&) = delete;
    AssetManager& operator=(const AssetManager&) = delete;

    // Texture
    TextureHandle LoadTexture(RHI* rhi, const std::string& LogicalPath);
    Texture2D* GetTexture(TextureHandle Handle) const;
    void UnloadTexture(TextureHandle Handle);

    // Shader
    ShaderHandle LoadShader(RHI* rhi, const std::string& VsPath, const std::string& FsPath);
    ShaderProgram* GetShader(ShaderHandle Handle) const;
    void UnloadShader(ShaderHandle Handle);

    // Bulk
    void UnloadAll();

    // Stats
    size_t GetTextureCount() const { return m_Textures.size(); }
    size_t GetShaderCount() const  { return m_Shaders.size(); }

private:
    AssetManager() = default;

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

    std::vector<TextureEntry> m_Textures;
    std::vector<ShaderEntry> m_Shaders;
};

} // namespace AMEE

#endif // __AMEE_ASSETMANAGER_H__
