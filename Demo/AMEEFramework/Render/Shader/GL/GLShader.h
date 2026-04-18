#pragma once
#include "../ShaderProgram.h"
#define GL_SILENCE_DEPRECATION
#include <OpenGL/gl.h>
#include <unordered_map>

class GLShaderProgram : public ShaderProgram {
public:
    GLShaderProgram();
    ~GLShaderProgram() override;

    bool compileFromSource(
        ShaderType type,
        const char* source,
        std::function<void(const ShaderCompileError&)> onError = nullptr
    ) override;

    bool link(std::function<void(const std::string&)> onError = nullptr) override;
    void use() override;
    void destroy() override;

    bool hasUniform(const std::string& name) const override;
    int  getUniformLocation(const std::string& name) const override;

    void setFloat (const std::string& name, float v) override;
    void setFloat2(const std::string& name, float x, float y) override;
    void setFloat3(const std::string& name, float x, float y, float z) override;
    void setFloat4(const std::string& name, float x, float y, float z, float w) override;
    void setInt   (const std::string& name, int v) override;
    void setMat4  (const std::string& name, const float* data) override;

    std::vector<UniformDesc> getUniforms() const override;

private:
    GLuint _programId;
    GLuint _vertShader;
    GLuint _fragShader;
    std::unordered_map<std::string, int> _uniformCache;

    int cacheUniformLocation(const std::string& name);
    static GLenum shaderTypeToGL(ShaderType type);
};
