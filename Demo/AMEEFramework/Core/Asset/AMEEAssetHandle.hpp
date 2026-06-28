#ifndef __AMEE_ASSETHANDLE_H__
#define __AMEE_ASSETHANDLE_H__
#pragma once
#include <cstdint>

namespace AMEE {

template<typename Tag>
struct AssetHandle {
    uint32_t Index = UINT32_MAX;

    bool IsValid() const { return Index != UINT32_MAX; }
    bool operator==(const AssetHandle& Other) const { return Index == Other.Index; }
    bool operator!=(const AssetHandle& Other) const { return Index != Other.Index; }
};

struct TextureTag {};
struct ShaderTag {};
struct MeshTag {};

using TextureHandle = AssetHandle<TextureTag>;
using ShaderHandle = AssetHandle<ShaderTag>;
using MeshHandle = AssetHandle<MeshTag>;

} // namespace AMEE

#endif // __AMEE_ASSETHANDLE_H__
