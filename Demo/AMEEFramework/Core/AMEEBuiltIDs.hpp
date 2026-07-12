#ifndef __AMEE_BUILTIDS_H__
#define __AMEE_BUILTIDS_H__
#pragma once
#include <cstdint>

namespace AMEE {

// 内建资源 ID 基址 (IDType::Resource = 0x7...)
constexpr uint64_t ID_BUILT_BASE = 0x7000000000000000ULL;
constexpr uint64_t ID_USER_BASE  = 0x7800000000000000ULL;  // 用户资源从这里开始

enum class BuiltID : uint64_t {
    // Shaders (0x7...01xxxxxx)
    Shader_Default  = ID_BUILT_BASE + 0x01000001,
    Shader_Unlit    = ID_BUILT_BASE + 0x01000002,
    Shader_Skybox   = ID_BUILT_BASE + 0x01000003,

    // Textures (0x7...02xxxxxx)
    Texture_White   = ID_BUILT_BASE + 0x02000001,
    Texture_Black   = ID_BUILT_BASE + 0x02000002,
    Texture_Normal  = ID_BUILT_BASE + 0x02000003,  // 法线贴图 (128,128,255)
    Texture_Missing = ID_BUILT_BASE + 0x02000004,  // 粉色缺失纹理
    Texture_Default = ID_BUILT_BASE + 0x02000005,  // 浅灰色 (192,192,192)

    // Materials (0x7...03xxxxxx)
    Material_Default = ID_BUILT_BASE + 0x03000001,
    Material_Missing = ID_BUILT_BASE + 0x03000002,
    Material_Skybox  = ID_BUILT_BASE + 0x03000010,
};

} // namespace AMEE

#endif // __AMEE_BUILTIDS_H__
