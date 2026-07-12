#ifndef __AMEE_ASSETHANDLE_H__
#define __AMEE_ASSETHANDLE_H__
#pragma once
#include "../AMEEGenerateUtils.hpp"

namespace AMEE {

//
enum class AssetType : uint32_t {
    None    = 0x0000,
    Texture = 0x0100,
    Cubemap = 0x0102,
    Shader  = 0x0200,
    Mesh    = 0x0300,
    Material= 0x0400,
};

struct AssetHandle {
    uint32_t Index = UINT32_MAX;
    uint64_t ID = ID_NULL;
    AssetType Type = AssetType::Texture;
    bool IsBuiltIn = false;
    
    bool IsValid() const { return Index != UINT32_MAX; }
    
    // Cubemap 是 Texture 的子类型
    bool IsTexture() const { return Type == AssetType::Texture || Type == AssetType::Cubemap; }
    bool IsCubemap() const { return Type == AssetType::Cubemap; }
    
    bool operator==(const AssetHandle& O) const { return Index == O.Index && Type == O.Type; }
    bool operator!=(const AssetHandle& O) const { return !(*this == O); }
    
    static AssetHandle Make(uint32_t InIndex, AssetType InType = AssetType::None,
                            uint64_t InID = ID_NULL, bool InIsBuiltIn = false) {
        return {InIndex, InID, InType, InIsBuiltIn};
    }
};

// 类型别名（语义区分）
using TextureHandle = AssetHandle;
using CubemapHandle = AssetHandle;
using ShaderHandle = AssetHandle;
using MeshHandle = AssetHandle;
using MaterialHandle = AssetHandle;

} // namespace AMEE

#endif // __AMEE_ASSETHANDLE_H__
