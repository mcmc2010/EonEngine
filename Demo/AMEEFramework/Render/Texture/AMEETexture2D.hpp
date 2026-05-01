#ifndef __AMEE_TEXTURE2D_H__
#define __AMEE_TEXTURE2D_H__
#pragma once
#include "../AMEERHI.hpp"
#include "AMEEImage.hpp"
#include <cstdint>
#include <string>

namespace AMEE {

class RHI;

class Texture2D {
public:
    Texture2D() = default;
    ~Texture2D();

    // Load from file and upload to GPU
    bool Load(RHI* rhi, const std::string& filePath);

    // Create from CPU data
    bool Create(RHI* rhi, const ImageData& image,
                RHIFormat format = RHIFormat::RGBA8);

    // Bind to texture unit
    void Bind(uint32_t slot = 0) const;

    // Set sampling parameters
    void SetFilter(RHIFilter minFilter, RHIFilter magFilter);
    void SetWrap(RHIWrap wrapS, RHIWrap wrapT);

    // Destroy
    void Destroy();

    // Accessors
    uint32_t GetTextureID() const { return m_TextureID; }
    int GetWidth()  const { return m_Width; }
    int GetHeight() const { return m_Height; }

private:
    RHI* m_pRHI = nullptr;
    uint32_t m_TextureID = 0;
    int m_Width  = 0;
    int m_Height = 0;
    RHIFormat m_Format = RHIFormat::RGBA8;
};

} // namespace AMEE

#endif // __AMEE_TEXTURE2D_H__