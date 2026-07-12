#include "AMEEAssetManager.hpp"
#include "AMEEFileSystem.hpp"
#include "../Log/AMEELog.hpp"
#include "../../Render/AMEERHI.hpp"
#include "../../Render/Texture/AMEETexture2D.hpp"
#include "../../Render/Texture/AMEEImage.hpp"
#include "../../Render/Shader/AMEEShaderProgram.hpp"
#include "../../Render/AMEEMesh.hpp"
#include "../../Render/Model/AMEEObjLoader.hpp"
#include "../../Render/Material/AMEEMaterial.hpp"
#include "../../Render/Material/AMEEStandardMaterial.hpp"
#include "../../Render/Material/AMEEStandardMaterialImporter.hpp"


namespace AMEE {

AMEE_SINGLETON_IMPL(AssetManager)

// ─── Built-in Resources ──────────────────────────────────────────────────────

void AssetManager::InitializeBuiltins(RHI* rhi)
{
    if (!rhi) {
        AMEE_LOG_ERROR("AssetManager", "Cannot initialize builtins: RHI is null");
        return;
    }

    InitBuiltinTextures(rhi);
    InitBuiltinShaders(rhi);
    InitBuiltinMaterials(rhi);

    AMEE_LOG_INFO("AssetManager", "Built-in resources initialized");
}

void AssetManager::InitBuiltinTextures(RHI* rhi)
{
    // 1x1 White texture (255, 255, 255, 255)
    {
        uint8_t pixels[] = { 255, 255, 255, 255 };
        auto tex = std::make_unique<Texture2D>();
        if (tex->Create(rhi, {std::vector<uint8_t>(pixels, pixels + 4), 1, 1, 4})) {
            uint64_t id = static_cast<uint64_t>(BuiltID::Texture_White);
            TextureHandle h = RegisterTexture(std::move(tex), "_Builtin_White", id, true);
            m_BuiltinTextures[id] = h;
        }
    }

    // 1x1 Black texture (0, 0, 0, 255)
    {
        uint8_t pixels[] = { 0, 0, 0, 255 };
        auto tex = std::make_unique<Texture2D>();
        if (tex->Create(rhi, {std::vector<uint8_t>(pixels, pixels + 4), 1, 1, 4})) {
            uint64_t id = static_cast<uint64_t>(BuiltID::Texture_Black);
            TextureHandle h = RegisterTexture(std::move(tex), "_Builtin_Black", id, true);
            m_BuiltinTextures[id] = h;
        }
    }

    // 1x1 Normal map texture (128, 128, 255, 255) - pointing up
    {
        uint8_t pixels[] = { 128, 128, 255, 255 };
        auto tex = std::make_unique<Texture2D>();
        if (tex->Create(rhi, {std::vector<uint8_t>(pixels, pixels + 4), 1, 1, 4})) {
            uint64_t id = static_cast<uint64_t>(BuiltID::Texture_Normal);
            TextureHandle h = RegisterTexture(std::move(tex), "_Builtin_Normal", id, true);
            m_BuiltinTextures[id] = h;
        }
    }

    // 1x1 Missing texture (magenta/pink) (255, 0, 255, 255)
    {
        uint8_t pixels[] = { 255, 0, 255, 255 };
        auto tex = std::make_unique<Texture2D>();
        if (tex->Create(rhi, {std::vector<uint8_t>(pixels, pixels + 4), 1, 1, 4})) {
            uint64_t id = static_cast<uint64_t>(BuiltID::Texture_Missing);
            TextureHandle h = RegisterTexture(std::move(tex), "_Builtin_Missing", id, true);
            m_BuiltinTextures[id] = h;
        }
    }

    // 1x1 Light gray texture (192, 192, 192, 255) (Default)
    {
        uint8_t pixels[] = { 192, 192, 192, 255 };
        auto tex = std::make_unique<Texture2D>();
        if (tex->Create(rhi, {std::vector<uint8_t>(pixels, pixels + 4), 1, 1, 4})) {
            uint64_t id = static_cast<uint64_t>(BuiltID::Texture_Default);
            TextureHandle h = RegisterTexture(std::move(tex), "_Builtin_Default", id, true);
            m_BuiltinTextures[id] = h;
        }
    }

    AMEE_LOG_INFO("AssetManager", "Built-in textures: %zu", m_BuiltinTextures.size());
}

void AssetManager::InitBuiltinShaders(RHI* rhi)
{
    // Default shader (vertex color + MVP)
    {
        uint64_t id = static_cast<uint64_t>(BuiltID::Shader_Default);
        ShaderHandle h = LoadShader(rhi, "Shaders/Default.vert", "Shaders/Default.frag", "_Built_Default", id, true);
        if (h.IsValid()) {
            m_BuiltinShaders[id] = h;
        }
    }

    // Unlit texture shader
    {
        uint64_t id = static_cast<uint64_t>(BuiltID::Shader_Unlit);
        ShaderHandle h = LoadShader(rhi, "Shaders/Unlit.vert", "Shaders/Unlit.frag", "_Built_Unlit", id, true);
        if (h.IsValid()) {
            m_BuiltinShaders[id] = h;
        }
    }

    // Skybox cubemap shader
    {
        uint64_t id = static_cast<uint64_t>(BuiltID::Shader_Skybox);
        ShaderHandle h = LoadShader(rhi, "Shaders/Skybox.vert", "Shaders/Skybox.frag", "_Built_Skybox", id, true);
        if (h.IsValid()) {
            m_BuiltinShaders[id] = h;
        }
    }

    AMEE_LOG_INFO("AssetManager", "Built-in shaders: %zu", m_BuiltinShaders.size());
}

void AssetManager::InitBuiltinMaterials(RHI* rhi)
{
    // Default material (PBR Standard + gray texture)
    {
        auto mat = std::make_unique<StandardMaterial>();
        mat->SetName("_Builtin_Default");
        mat->SetShader(GetBuiltinShader(BuiltID::Shader_Default));
        mat->SetAlbedoMap(GetBuiltinTexture(BuiltID::Texture_Default)); // 浅灰纹理
        mat->SetNormalMap(GetBuiltinTexture(BuiltID::Texture_Normal));
        
        uint64_t id = static_cast<uint64_t>(BuiltID::Material_Default);
        MaterialHandle h = RegisterMaterial(std::move(mat), id, true);
        m_BuiltinMaterials[id] = h;
    }

    // Missing material (uses unlit shader + missing texture)
    {
        auto mat = std::make_unique<Material>();
        mat->SetName("_Builtin_Missing");
        mat->SetShader(GetBuiltinShader(BuiltID::Shader_Unlit));
        mat->SetTexture("u_MainTex", GetBuiltinTexture(BuiltID::Texture_Missing));
        uint64_t id = static_cast<uint64_t>(BuiltID::Material_Missing);
        MaterialHandle h = RegisterMaterial(std::move(mat), id, true);
        m_BuiltinMaterials[id] = h;
    }

    // Skybox material (uses skybox shader, cubemap will be set at runtime)
    {
        auto mat = std::make_unique<Material>();
        mat->SetName("_Builtin_Skybox");
        mat->SetShader(GetBuiltinShader(BuiltID::Shader_Skybox));
        uint64_t id = static_cast<uint64_t>(BuiltID::Material_Skybox);
        MaterialHandle h = RegisterMaterial(std::move(mat), id, true);
        m_BuiltinMaterials[id] = h;
    }

    AMEE_LOG_INFO("AssetManager", "Built-in materials: %zu", m_BuiltinMaterials.size());
}

ShaderHandle AssetManager::GetBuiltinShader(BuiltID id) const
{
    auto it = m_BuiltinShaders.find(static_cast<uint64_t>(id));
    return (it != m_BuiltinShaders.end()) ? it->second : ShaderHandle{};
}

TextureHandle AssetManager::GetBuiltinTexture(BuiltID id) const
{
    auto it = m_BuiltinTextures.find(static_cast<uint64_t>(id));
    return (it != m_BuiltinTextures.end()) ? it->second : TextureHandle{};
}

MaterialHandle AssetManager::GetBuiltinMaterial(BuiltID id) const
{
    auto it = m_BuiltinMaterials.find(static_cast<uint64_t>(id));
    return (it != m_BuiltinMaterials.end()) ? it->second : MaterialHandle{};
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

    std::string PhysicalPath = FileSystem::GetSingleton().ResolvePath(LogicalPath);
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

TextureHandle AssetManager::RegisterTexture(std::unique_ptr<Texture2D> InTex, const std::string& Name, uint64_t ID, bool IsBuiltIn)
{
    if (!InTex) return {};
    uint32_t Idx = static_cast<uint32_t>(m_Textures.size());
    m_Textures.push_back({std::move(InTex), Name, 1});
    AMEE_LOG_INFO("AssetManager", "Registered texture [%u]: %s", Idx, Name.c_str());
    return TextureHandle::Make(Idx, AssetType::Texture, ID, IsBuiltIn);
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

// ─── Cubemap ─────────────────────────────────────────────────────────────────

CubemapHandle AssetManager::LoadCubemap(RHI* rhi, const std::string& Dir,
                                         const std::string& PosX, const std::string& NegX,
                                         const std::string& PosY, const std::string& NegY,
                                         const std::string& PosZ, const std::string& NegZ)
{
    if (!rhi) return {};

    std::string Base = Dir;
    if (!Base.empty() && Base.back() != '/') Base += '/';

    std::string Paths[6] = { Base + PosX, Base + NegX, Base + PosY, Base + NegY, Base + PosZ, Base + NegZ };
    const unsigned char* FaceData[6] = {};
    std::vector<ImageData> Images(6);
    int FaceW = 0, FaceH = 0;

    for (int i = 0; i < 6; i++) {
        std::string ResolvedPath = FileSystem::GetSingleton().ResolvePath(Paths[i]);
        SetImageFlipVertical(false);
        Images[i] = LoadImage(ResolvedPath.empty() ? Paths[i] : ResolvedPath);
        SetImageFlipVertical(true);
        if (Images[i].Pixels.empty()) {
            AMEE_LOG_ERROR("AssetManager", "Failed to load cubemap face: %s", Paths[i].c_str());
            return {};
        }
        FaceW = Images[i].Width;
        FaceH = Images[i].Height;
        FaceData[i] = Images[i].Pixels.data();
    }

    uint32_t glID = rhi->createCubemap(FaceData, FaceW, FaceH, RHIFormat::RGBA8, RHIFormat::RGBA8);
    if (glID == 0) {
        AMEE_LOG_ERROR("AssetManager", "Failed to create cubemap from %s", Dir.c_str());
        return {};
    }

    CubemapHandle Handle = RegisterCubemap(glID, Dir);
    AMEE_LOG_INFO("AssetManager", "Loaded cubemap [%u] from %s (%dx%d)", Handle.Index, Dir.c_str(), FaceW, FaceH);
    return Handle;
}

CubemapHandle AssetManager::LoadCubemapCross(RHI* rhi, const std::string& FilePath)
{
    if (!rhi) return {};

    std::string ResolvedPath = FileSystem::GetSingleton().ResolvePath(FilePath);
    SetImageFlipVertical(false);
    ImageData img = LoadImage(ResolvedPath.empty() ? FilePath : ResolvedPath);
    SetImageFlipVertical(true);
    if (img.Pixels.empty()) {
        AMEE_LOG_ERROR("AssetManager", "Failed to load cubemap cross: %s", FilePath.c_str());
        return {};
    }

    // Detect layout: horizontal cross (4:3) or vertical cross (3:4)
    bool isHorizontal = (img.Width * 3 == img.Height * 4);
    bool isVertical = (img.Width * 4 == img.Height * 3);

    if (!isHorizontal && !isVertical) {
        AMEE_LOG_ERROR("AssetManager", "Invalid cubemap cross aspect ratio (%dx%d), expected 4:3 or 3:4",
                       img.Width, img.Height);
        return {};
    }

    int faceW, faceH;
    // Face offsets for horizontal cross layout:
    //       [PY]
    // [NX] [PZ] [PX] [NZ]
    //       [NY]
    int offsets[6][2]; // [face][x, y]

    if (isHorizontal) {
        faceW = img.Width / 4;
        faceH = img.Height / 3;
        // +X: col 2, row 1
        offsets[0][0] = 2 * faceW; offsets[0][1] = 1 * faceH;
        // -X: col 0, row 1
        offsets[1][0] = 0;         offsets[1][1] = 1 * faceH;
        // +Y: col 1, row 0
        offsets[2][0] = 1 * faceW; offsets[2][1] = 0;
        // -Y: col 1, row 2
        offsets[3][0] = 1 * faceW; offsets[3][1] = 2 * faceH;
        // +Z: col 1, row 1
        offsets[4][0] = 1 * faceW; offsets[4][1] = 1 * faceH;
        // -Z: col 3, row 1
        offsets[5][0] = 3 * faceW; offsets[5][1] = 1 * faceH;
    } else {
        // Vertical cross layout:
        // [PY]
        // [PX]
        // [PZ]
        // [NY]
        faceW = img.Width / 3;
        faceH = img.Height / 4;
        offsets[0][0] = 2 * faceW; offsets[0][1] = 1 * faceH; // +X
        offsets[1][0] = 0;         offsets[1][1] = 1 * faceH; // -X
        offsets[2][0] = 1 * faceW; offsets[2][1] = 0;         // +Y
        offsets[3][0] = 1 * faceW; offsets[3][1] = 2 * faceH; // -Y
        offsets[4][0] = 1 * faceW; offsets[4][1] = 1 * faceH; // +Z
        offsets[5][0] = 1 * faceW; offsets[5][1] = 3 * faceH; // -Z
    }

    // Cut 6 faces from the cross image (skip 1-pixel border to avoid seams)
    int border = 1; // Skip 1 pixel at each edge
    int innerW = faceW - 2 * border;
    int innerH = faceH - 2 * border;

    std::vector<uint8_t> faceBuffers[6];
    const unsigned char* faceData[6];
    int bpp = 4; // RGBA

    for (int f = 0; f < 6; f++) {
        faceBuffers[f].resize(innerW * innerH * bpp);
        int ox = offsets[f][0] + border;
        int oy = offsets[f][1] + border;

        for (int y = 0; y < innerH; y++) {
            int srcRow = oy + y;
            int srcCol = ox;
            int srcOffset = (srcRow * img.Width + srcCol) * bpp;
            int dstOffset = y * innerW * bpp;
            std::memcpy(faceBuffers[f].data() + dstOffset,
                       img.Pixels.data() + srcOffset,
                       innerW * bpp);
        }
        faceData[f] = faceBuffers[f].data();
    }

    uint32_t glID = rhi->createCubemap(faceData, innerW, innerH, RHIFormat::RGBA8, RHIFormat::RGBA8);
    if (glID == 0) {
        AMEE_LOG_ERROR("AssetManager", "Failed to create cubemap from cross image");
        return {};
    }

    CubemapHandle Handle = RegisterCubemap(glID, FilePath);
    AMEE_LOG_INFO("AssetManager", "Loaded cubemap cross [%u] from %s (%dx%d, each face %dx%d, border skipped %dpx)",
                  Handle.Index, FilePath.c_str(), img.Width, img.Height, innerW, innerH, border);
    return Handle;
}

CubemapHandle AssetManager::RegisterCubemap(uint32_t GLID, const std::string& Name, uint64_t ID, bool IsBuiltIn)
{
    if (GLID == 0) return {};
    uint32_t Idx = static_cast<uint32_t>(m_Cubemaps.size());
    m_Cubemaps.push_back({GLID, Name, 1});
    AMEE_LOG_INFO("AssetManager", "Registered cubemap [%u]: %s", Idx, Name.c_str());
    return CubemapHandle::Make(Idx, AssetType::Cubemap, ID, IsBuiltIn);
}

uint32_t AssetManager::GetCubemap(CubemapHandle Handle) const
{
    if (!Handle.IsValid() || Handle.Index >= m_Cubemaps.size()) return 0;
    return m_Cubemaps[Handle.Index].GLID;
}

void AssetManager::UnloadCubemap(CubemapHandle Handle)
{
    if (!Handle.IsValid() || Handle.Index >= m_Cubemaps.size()) return;
    auto& Entry = m_Cubemaps[Handle.Index];
    if (Entry.RefCount == 0) return;
    Entry.RefCount--;
    if (Entry.RefCount == 0) {
        AMEE_LOG_INFO("AssetManager", "Unloaded cubemap [%u]: %s", Handle.Index, Entry.Name.c_str());
        Entry.GLID = 0;
    }
}

// ─── Shader ────────────────────────────────────────────────────────────────────

ShaderHandle AssetManager::LoadShader(RHI* rhi, const std::string& VsPath, const std::string& FsPath, const std::string& Name, uint64_t ID, bool IsBuiltIn)
{
    if (!rhi) return {};

    // Ensure built-in resources have Assets/ prefix
    auto EnsureAssetsPrefix = [](const std::string& Path) -> std::string {
        if (Path.find("Assets/") == 0) return Path;
        return "Assets/" + Path;
    };

    std::string ActualVsPath = IsBuiltIn ? EnsureAssetsPrefix(VsPath) : VsPath;
    std::string ActualFsPath = IsBuiltIn ? EnsureAssetsPrefix(FsPath) : FsPath;

    for (size_t i = 0; i < m_Shaders.size(); ++i) {
        if (m_Shaders[i].VsPath == ActualVsPath && m_Shaders[i].FsPath == ActualFsPath && m_Shaders[i].Resource) {
            m_Shaders[i].RefCount++;
            return ShaderHandle::Make(static_cast<uint32_t>(i), AssetType::Shader);
        }
    }

    std::string VsSource = FileSystem::GetSingleton().ReadText(ActualVsPath);
    std::string FsSource = FileSystem::GetSingleton().ReadText(ActualFsPath);
    if (VsSource.empty() || FsSource.empty()) {
        AMEE_LOG_ERROR("AssetManager", "Failed to read shader sources: %s / %s",
                       ActualVsPath.c_str(), ActualFsPath.c_str());
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

    std::string RegisterName = Name.empty() ? (ActualVsPath + " + " + ActualFsPath) : Name;
    return RegisterShader(std::move(Shader), RegisterName, ID, IsBuiltIn);
}

ShaderHandle AssetManager::RegisterShader(std::unique_ptr<ShaderProgram> InShader, const std::string& Name, uint64_t ID, bool IsBuiltIn)
{
    if (!InShader) return {};
    uint32_t Idx = static_cast<uint32_t>(m_Shaders.size());
    m_Shaders.push_back({std::move(InShader), Name, "", 1});
    AMEE_LOG_INFO("AssetManager", "Registered shader [%u]: %s", Idx, Name.c_str());
    return ShaderHandle::Make(Idx, AssetType::Shader, ID, IsBuiltIn);
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

MeshHandle AssetManager::RegisterMesh(std::unique_ptr<Mesh> InMesh, const std::string& Name, uint64_t ID, bool IsBuiltIn)
{
    if (!InMesh) return {};
    uint32_t Idx = static_cast<uint32_t>(m_Meshes.size());
    m_Meshes.push_back({std::move(InMesh), Name, 1});
    AMEE_LOG_INFO("AssetManager", "Registered mesh [%u]: %s", Idx, Name.c_str());
    return MeshHandle::Make(Idx, AssetType::Mesh, ID, IsBuiltIn);
}

MeshHandle AssetManager::LoadModel(RHI* rhi, const std::string& LogicalPath,
                                   std::vector<MaterialHandle>* OutMaterials)
{
    if (!rhi) return {};

    std::string Source = FileSystem::GetSingleton().ReadText(LogicalPath);
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
        std::string MtlSource = FileSystem::GetSingleton().ReadText(MtlPath);

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

MaterialHandle AssetManager::RegisterMaterial(std::unique_ptr<Material> InMat, uint64_t ID, bool IsBuiltIn)
{
    if (!InMat) return {};
    uint32_t Idx = static_cast<uint32_t>(m_Materials.size());
    m_Materials.push_back({std::move(InMat), "", 1});
    AMEE_LOG_INFO("AssetManager", "Registered material [%u]: %s", Idx, m_Materials.back().Resource->GetName().c_str());
    return MaterialHandle::Make(Idx, AssetType::Material, ID, IsBuiltIn);
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
    m_Cubemaps.clear();
    m_Shaders.clear();
    m_Meshes.clear();
    m_Materials.clear();
    AMEE_LOG_INFO("AssetManager", "All assets unloaded");
}

} // namespace AMEE
