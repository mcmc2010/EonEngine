#include "AMEEImage.hpp"

// stb_image: must define in .cpp
#define STB_IMAGE_IMPLEMENTATION
#include "../../../ThirdParty/stb/stb_image.h"

#include "../../Core/Log/AMEELog.hpp"

namespace AMEE {

ImageData LoadImage(const std::string& FilePath)
{
    ImageData result;

    int channels = 0;
    unsigned char* data = stbi_load(FilePath.c_str(), &result.Width, &result.Height, &channels, 4);
    if (!data) {
        AMEE_LOG_ERROR("Image", "Failed to load image: %s (%s)",
                       FilePath.c_str(), stbi_failure_reason());
        return result;
    }

    int pixelCount = result.Width * result.Height * 4;
    result.Pixels.resize(pixelCount);
    std::memcpy(result.Pixels.data(), data, pixelCount);
    result.Channels = 4;

    stbi_image_free(data);

    AMEE_LOG_INFO("Image", "Loaded image: %s (%dx%d)",
                  FilePath.c_str(), result.Width, result.Height);
    return result;
}

} // namespace AMEE