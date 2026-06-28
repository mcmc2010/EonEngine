#include "AMEEAssetManager.hpp"
#include "AMEEFileSystem.hpp"
#include "../Log/AMEELog.hpp"
#include "../../Render/AMEERHI.hpp"
#include "../../Render/Texture/AMEETexture2D.hpp"
#include "../../Render/Shader/AMEEShaderProgram.hpp"

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

    uint32_t Idx = static_cast<uint32_t>(m_Textures.size());
    m_Textures.push_back({std::move(Tex), LogicalPath, 1});
    AMEE_LOG_INFO("AssetManager", "Loaded texture [%u]: %s", Idx, LogicalPath.c_str());
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

    uint32_t Idx = static_cast<uint32_t>(m_Shaders.size());
    m_Shaders.push_back({std::move(Shader), VsPath, FsPath, 1});
    AMEE_LOG_INFO("AssetManager", "Loaded shader [%u]: %s + %s", Idx, VsPath.c_str(), FsPath.c_str());
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

// ─── Bulk ──────────────────────────────────────────────────────────────────────

void AssetManager::UnloadAll()
{
    m_Textures.clear();
    m_Shaders.clear();
    AMEE_LOG_INFO("AssetManager", "All assets unloaded");
}

} // namespace AMEE
