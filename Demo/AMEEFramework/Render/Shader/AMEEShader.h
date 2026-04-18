#pragma once
#include <cstdint>
#include <string>

namespace AMEE {

enum class ShaderType : uint8_t {
    Vertex,
    Fragment,
};

struct ShaderCompileError {
    std::string message;
    int line;
};

using ShaderHandle = void*;
}