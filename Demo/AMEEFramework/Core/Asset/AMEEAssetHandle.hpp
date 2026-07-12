#ifndef __AMEE_ASSETHANDLE_H__
#define __AMEE_ASSETHANDLE_H__
#pragma once
#include "../AMEEGenerateUtils.hpp"

namespace AMEE {

template<typename Tag>
struct AssetHandle {
    uint32_t Index = UINT32_MAX;
    uint64_t ID = ID_NULL;
    bool IsBuiltIn = false;
    
    bool IsValid() const { return Index != UINT32_MAX; }
    
    bool operator==(const AssetHandle& O) const { return Index == O.Index; }
    bool operator!=(const AssetHandle& O) const { return Index != O.Index; }
    
    static AssetHandle Make(uint32_t InIndex, uint64_t InID = ID_NULL, bool InIsBuiltIn = false) {
        return {InIndex, InID, InIsBuiltIn};
    }
};

struct TextureTag {};
struct ShaderTag {};
struct MeshTag {};
struct MaterialTag {};

using TextureHandle = AssetHandle<TextureTag>;
using ShaderHandle = AssetHandle<ShaderTag>;
using MeshHandle = AssetHandle<MeshTag>;
using MaterialHandle = AssetHandle<MaterialTag>;

} // namespace AMEE

#endif // __AMEE_ASSETHANDLE_H__
