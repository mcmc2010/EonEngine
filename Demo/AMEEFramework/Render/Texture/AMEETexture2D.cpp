#include "AMEETexture2D.hpp"
#include "../AMEERHI.hpp"
#include "../../Core/Log/AMEELog.hpp"

namespace AMEE {

Texture2D::~Texture2D() { Destroy(); }

bool Texture2D::Load(RHI* rhi, const std::string& filePath)
{
    ImageData image = LoadImage(filePath);
    if (image.Pixels.empty()) return false;

    return Create(rhi, image);
}

bool Texture2D::Create(RHI* rhi, const ImageData& image, RHIFormat format)
{
    if (!rhi || image.Pixels.empty() || image.Width <= 0 || image.Height <= 0) {
        AMEE_LOG_ERROR("Texture2D", "Invalid texture parameters");
        return false;
    }

    Destroy();

    m_pRHI = rhi;
    m_Width = image.Width;
    m_Height = image.Height;
    m_Format = format;

    m_TextureID = rhi->createTexture(image.Pixels.data(), m_Width, m_Height,
                                      RHIFormat::RGBA8, format);
    if (m_TextureID == 0) {
        AMEE_LOG_ERROR("Texture2D", "Failed to create GPU texture");
        return false;
    }

    AMEE_LOG_INFO("Texture2D", "Created texture (%dx%d, id=%u)",
                  m_Width, m_Height, m_TextureID);
    return true;
}

void Texture2D::Bind(uint32_t slot) const
{
    if (m_pRHI && m_TextureID) {
        m_pRHI->bindTexture(m_TextureID, slot);
    }
}

void Texture2D::SetFilter(RHIFilter minFilter, RHIFilter magFilter)
{
    if (m_pRHI && m_TextureID) {
        m_pRHI->setTextureFilter(m_TextureID, minFilter, magFilter);
    }
}

void Texture2D::SetWrap(RHIWrap wrapS, RHIWrap wrapT)
{
    if (m_pRHI && m_TextureID) {
        m_pRHI->setTextureWrap(m_TextureID, wrapS, wrapT);
    }
}

void Texture2D::Destroy()
{
    if (m_pRHI && m_TextureID) {
        m_pRHI->destroyTexture(m_TextureID);
        m_TextureID = 0;
    }
    m_pRHI = nullptr;
    m_Width = m_Height = 0;
}

} // namespace AMEE