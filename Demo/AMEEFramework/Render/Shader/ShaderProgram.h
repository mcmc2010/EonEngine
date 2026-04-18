#pragma once
#include "Shader.h"
#include <string>
#include <vector>
#include <functional>

enum class UniformType : uint8_t {
    Float,
    Float2,
    Float3,
    Float4,
    Int,
    Mat4,
};

struct UniformDesc {
    std::string name;
    UniformType type;
    int location;
};

class ShaderProgram {
public:
    virtual ~ShaderProgram() = default;

    virtual bool compileFromSource(
        ShaderType type,
        const char* source,
        std::function<void(const ShaderCompileError&)> onError = nullptr
    ) = 0;

    virtual bool link(std::function<void(const std::string&)> onError = nullptr) = 0;
    virtual void use() = 0;
    virtual void destroy() = 0;

    virtual bool hasUniform(const std::string& name) const = 0;
    virtual int  getUniformLocation(const std::string& name) const = 0;

    virtual void setFloat (const std::string& name, float v) = 0;
    virtual void setFloat2(const std::string& name, float x, float y) = 0;
    virtual void setFloat3(const std::string& name, float x, float y, float z) = 0;
    virtual void setFloat4(const std::string& name, float x, float y, float z, float w) = 0;
    virtual void setInt   (const std::string& name, int v) = 0;
    virtual void setMat4  (const std::string& name, const float* data) = 0;

    virtual std::vector<UniformDesc> getUniforms() const = 0;
};
