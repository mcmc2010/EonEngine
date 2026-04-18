#include "AMEEGLShader.h"
#include <vector>

namespace AMEE {

GLShaderProgram::GLShaderProgram()
    : _programId(0)
    , _vertShader(0)
    , _fragShader(0)
{
    _programId = glCreateProgram();
}

GLShaderProgram::~GLShaderProgram()
{
    destroy();
}

GLenum GLShaderProgram::shaderTypeToGL(ShaderType type)
{
    switch (type) {
        case ShaderType::Vertex:   return GL_VERTEX_SHADER;
        case ShaderType::Fragment: return GL_FRAGMENT_SHADER;
    }
    return GL_VERTEX_SHADER;
}

bool GLShaderProgram::compileFromSource(
    ShaderType type,
    const char* source,
    std::function<void(const ShaderCompileError&)> onError)
{
    GLuint shader = glCreateShader(shaderTypeToGL(type));
    glShaderSource(shader, 1, &source, nullptr);
    glCompileShader(shader);

    GLint success = 0;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        GLint logLen = 0;
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &logLen);
        std::vector<GLchar> log(logLen);
        glGetShaderInfoLog(shader, logLen, nullptr, log.data());

        ShaderCompileError err;
        err.message = log.data();
        err.line = -1;

        if (onError) onError(err);
        glDeleteShader(shader);
        return false;
    }

    switch (type) {
        case ShaderType::Vertex:
            if (_vertShader) glDeleteShader(_vertShader);
            _vertShader = shader;
            break;
        case ShaderType::Fragment:
            if (_fragShader) glDeleteShader(_fragShader);
            _fragShader = shader;
            break;
    }

    return true;
}

bool GLShaderProgram::link(std::function<void(const std::string&)> onError)
{
    if (!_vertShader || !_fragShader) {
        if (onError) onError("Missing vertex or fragment shader");
        return false;
    }

    glAttachShader(_programId, _vertShader);
    glAttachShader(_programId, _fragShader);
    glLinkProgram(_programId);

    GLint success = 0;
    glGetProgramiv(_programId, GL_LINK_STATUS, &success);
    if (!success) {
        GLint logLen = 0;
        glGetProgramiv(_programId, GL_INFO_LOG_LENGTH, &logLen);
        std::vector<GLchar> log(logLen);
        glGetProgramInfoLog(_programId, logLen, nullptr, log.data());

        if (onError) onError(log.data());
        return false;
    }

    _uniformCache.clear();
    return true;
}

void GLShaderProgram::use()
{
    glUseProgram(_programId);
}

void GLShaderProgram::destroy()
{
    if (_vertShader) {
        glDeleteShader(_vertShader);
        _vertShader = 0;
    }
    if (_fragShader) {
        glDeleteShader(_fragShader);
        _fragShader = 0;
    }
    if (_programId) {
        glDeleteProgram(_programId);
        _programId = 0;
    }
    _uniformCache.clear();
}

int GLShaderProgram::cacheUniformLocation(const std::string& name)
{
    auto it = _uniformCache.find(name);
    if (it != _uniformCache.end()) return it->second;

    int loc = glGetUniformLocation(_programId, name.c_str());
    _uniformCache[name] = loc;
    return loc;
}

bool GLShaderProgram::hasUniform(const std::string& name) const
{
    return glGetUniformLocation(_programId, name.c_str()) != -1;
}

int GLShaderProgram::getUniformLocation(const std::string& name) const
{
    auto it = _uniformCache.find(name);
    if (it != _uniformCache.end()) return it->second;
    return glGetUniformLocation(_programId, name.c_str());
}

void GLShaderProgram::setFloat(const std::string& name, float v)
{
    glUniform1f(cacheUniformLocation(name), v);
}

void GLShaderProgram::setFloat2(const std::string& name, float x, float y)
{
    glUniform2f(cacheUniformLocation(name), x, y);
}

void GLShaderProgram::setFloat3(const std::string& name, float x, float y, float z)
{
    glUniform3f(cacheUniformLocation(name), x, y, z);
}

void GLShaderProgram::setFloat4(const std::string& name, float x, float y, float z, float w)
{
    glUniform4f(cacheUniformLocation(name), x, y, z, w);
}

void GLShaderProgram::setInt(const std::string& name, int v)
{
    glUniform1i(cacheUniformLocation(name), v);
}

void GLShaderProgram::setMat4(const std::string& name, const float* data)
{
    glUniformMatrix4fv(cacheUniformLocation(name), 1, GL_FALSE, data);
}

std::vector<UniformDesc> GLShaderProgram::getUniforms() const
{
    std::vector<UniformDesc> result;

    GLint count = 0;
    glGetProgramiv(_programId, GL_ACTIVE_UNIFORMS, &count);

    for (int i = 0; i < count; i++) {
        GLchar name[256];
        GLint size;
        GLenum type;
        glGetActiveUniform(_programId, i, sizeof(name), nullptr, &size, &type, name);

        UniformDesc desc;
        desc.name = name;
        desc.location = glGetUniformLocation(_programId, name);

        switch (type) {
            case GL_FLOAT:         desc.type = UniformType::Float; break;
            case GL_FLOAT_VEC2:    desc.type = UniformType::Float2; break;
            case GL_FLOAT_VEC3:    desc.type = UniformType::Float3; break;
            case GL_FLOAT_VEC4:    desc.type = UniformType::Float4; break;
            case GL_INT:           desc.type = UniformType::Int; break;
            case GL_FLOAT_MAT4:    desc.type = UniformType::Mat4; break;
            default:               desc.type = UniformType::Float; break;
        }

        result.push_back(desc);
    }

    return result;
}
}