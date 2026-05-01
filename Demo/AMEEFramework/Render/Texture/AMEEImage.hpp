#ifndef __AMEE_IMAGE_H__
#define __AMEE_IMAGE_H__
#pragma once
#include <string>
#include <vector>
#include <cstdint>

namespace AMEE {

struct ImageData {
    std::vector<uint8_t> Pixels;
    int Width  = 0;
    int Height = 0;
    int Channels = 4;
};

// CPU-side image loading (stb_image)
ImageData LoadImage(const std::string& FilePath);

} // namespace AMEE
#endif // __AMEE_IMAGE_H__