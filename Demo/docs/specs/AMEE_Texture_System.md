# 纹理系统开发规格说明书

## 总览

为 AMEEFramework 添加纹理加载与渲染能力。使用 stb_image.h 加载 PNG/JPEG 图片，扩展 RHI 接口支持纹理上传，创建 Texture2D 类封装纹理生命周期，并在 DemoApp 中渲染一个带纹理的四边形。

## 涉及文件

| 操作 | 文件 | 说明 | 状态 |
|------|------|------|------|
| **修改** | `Render/AMEERHI.hpp` | RHI 基类新增纹理虚函数 | ✅ |
| **修改** | `Render/AMEERHITypes.hpp` | 新增纹理相关的枚举/结构体 | ✅ |
| **修改** | `Platform/macOS/GL/AMEERHIOpenGL.hpp` | RHIOpenGL 实现新增纹理方法 | ✅ |
| **修改** | `Platform/macOS/GL/AMEERHIOpenGL.mm` | OpenGL 纹理创建/绑定实装 | ✅ |
| **新建** | `Render/Texture/AMEEImage.hpp` | stb_image 封装：CPU 侧图片数据 | ✅ |
| **新建** | `Render/Texture/AMEEImage.cpp` | 实现 LoadImage / FreeImage | ✅ |
| **新建** | `Render/Texture/AMEETexture2D.hpp` | GPU 纹理对象，Create/Destroy/Bind | ✅ |
| **新建** | `Render/Texture/AMEETexture2D.cpp` | 实现（调用 RHI 接口） | ✅ |
| **修改** | `Sources/DemoApp.hpp` | 新增成员 Texture2D + Mesh(四边形) | ❌ |
| **修改** | `Sources/DemoApp.cpp` | 初始化纹理+四边形，渲染调用 | ❌ |
| **新建** | `Demo/Assets/Textures/` | 放一张测试图片（如 checkboard.png） | ✅ |

---

## ✅ 步骤 1：扩展 RHI 接口（已完成）

### 1.1 RHI 基类新增纹理接口

文件：`Render/AMEERHI.hpp`

在 `AMEERHITypes.hpp` 中新增：

```cpp
// Render/AMEERHITypes.hpp - 在 namespace AMEE 内添加

enum class RHIFormat : uint8_t {
    R8,
    RG8,
    RGB8,
    RGBA8,
};

enum class RHIFilter : uint8_t {
    Nearest,
    Linear,
};

enum class RHIWrap : uint8_t {
    Repeat,
    ClampToEdge,
    MirroredRepeat,
};
```

在 `AMEERHI.hpp` 的 `class RHI` 中新增虚函数（放在 Draw 区域前）：

```cpp
class RHI {
public:
    // ... 现有方法 ...

    // Texture
    virtual uint32_t createTexture(const unsigned char* data, int width, int height,
                                    RHIFormat format, RHIFormat internalFormat) = 0;
    virtual void destroyTexture(uint32_t id) = 0;
    virtual void bindTexture(uint32_t id, uint32_t slot) = 0;
    virtual void setTextureFilter(uint32_t id, RHIFilter minFilter, RHIFilter magFilter) = 0;
    virtual void setTextureWrap(uint32_t id, RHIWrap wrapS, RHIWrap wrapT) = 0;

    // ... 现有方法 ...
};
```

参数说明：
- `data`：RGBA 裸像素数据（stb_image 输出格式）
- `width/height`：图片尺寸
- `format`：传入数据的像素格式（通常 RGBA8）
- `internalFormat`：GPU 存储格式（通常 RGBA8）
- `slot`：纹理单元索引（0-31）

### 1.2 RHIOpenGL 实现

文件：`Platform/macOS/GL/AMEERHIOpenGL.hpp`

新增 override 声明：

```cpp
// Texture
uint32_t createTexture(const unsigned char* data, int width, int height,
                        RHIFormat format, RHIFormat internalFormat) override;
void destroyTexture(uint32_t id) override;
void bindTexture(uint32_t id, uint32_t slot) override;
void setTextureFilter(uint32_t id, RHIFilter minFilter, RHIFilter magFilter) override;
void setTextureWrap(uint32_t id, RHIWrap wrapS, RHIWrap wrapT) override;
```

文件：`Platform/macOS/GL/AMEERHIOpenGL.mm`

实现（在 namespace AMEE 内）：

```cpp
// Texture
uint32_t RHIOpenGL::createTexture(const unsigned char* data, int width, int height,
                                    RHIFormat format, RHIFormat internalFormat)
{
    GLuint tex = 0;
    glGenTextures(1, &tex);
    glBindTexture(GL_TEXTURE_2D, tex);

    GLenum glFormat = RHIOpenGL::formatToGL(format);
    GLenum glInternal = RHIOpenGL::internalFormatToGL(internalFormat);

    glTexImage2D(GL_TEXTURE_2D, 0, glInternal, width, height, 0,
                 glFormat, GL_UNSIGNED_BYTE, data);

    // Default filtering
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    glBindTexture(GL_TEXTURE_2D, 0);
    return tex;
}

void RHIOpenGL::destroyTexture(uint32_t id)
{
    GLuint tex = id;
    glDeleteTextures(1, &tex);
}

void RHIOpenGL::bindTexture(uint32_t id, uint32_t slot)
{
    glActiveTexture(GL_TEXTURE0 + slot);
    glBindTexture(GL_TEXTURE_2D, id);
}

void RHIOpenGL::setTextureFilter(uint32_t id, RHIFilter minFilter, RHIFilter magFilter)
{
    glBindTexture(GL_TEXTURE_2D, id);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, RHIOpenGL::filterToGL(minFilter));
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, RHIOpenGL::filterToGL(magFilter));
    glBindTexture(GL_TEXTURE_2D, 0);
}

void RHIOpenGL::setTextureWrap(uint32_t id, RHIWrap wrapS, RHIWrap wrapT)
{
    glBindTexture(GL_TEXTURE_2D, id);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, RHIOpenGL::wrapToGL(wrapS));
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, RHIOpenGL::wrapToGL(wrapT));
    glBindTexture(GL_TEXTURE_2D, 0);
}
```

静态辅助方法（在 `RHIOpenGL` 类中新增）：

```cpp
// 头文件新增 private static 方法
private:
    static GLenum formatToGL(RHIFormat format);
    static GLenum internalFormatToGL(RHIFormat format);
    static GLenum filterToGL(RHIFilter filter);
    static GLenum wrapToGL(RHIWrap wrap);
```

实现：

```cpp
GLenum RHIOpenGL::formatToGL(RHIFormat format) {
    switch (format) {
        case RHIFormat::R8:    return GL_RED;
        case RHIFormat::RG8:   return GL_RG;
        case RHIFormat::RGB8:  return GL_RGB;
        case RHIFormat::RGBA8: return GL_RGBA;
    }
    return GL_RGBA;
}

GLenum RHIOpenGL::internalFormatToGL(RHIFormat format) {
    // OpenGL 3.x+ 用 sized format。macOS 4.1 支持
    switch (format) {
        case RHIFormat::R8:    return GL_R8;
        case RHIFormat::RG8:   return GL_RG8;
        case RHIFormat::RGB8:  return GL_RGB8;
        case RHIFormat::RGBA8: return GL_RGBA8;
    }
    return GL_RGBA8;
}

GLenum RHIOpenGL::filterToGL(RHIFilter filter) {
    switch (filter) {
        case RHIFilter::Nearest: return GL_NEAREST;
        case RHIFilter::Linear:  return GL_LINEAR;
    }
    return GL_LINEAR;
}

GLenum RHIOpenGL::wrapToGL(RHIWrap wrap) {
    switch (wrap) {
        case RHIWrap::Repeat:         return GL_REPEAT;
        case RHIWrap::ClampToEdge:    return GL_CLAMP_TO_EDGE;
        case RHIWrap::MirroredRepeat: return GL_MIRRORED_REPEAT;
    }
    return GL_REPEAT;
}
```

---

## ✅ 步骤 2：创建 Image 封装（已完成）

注意：实现了 `LoadImage()` 和 `ImageData` 结构体，`#define STB_IMAGE_IMPLEMENTATION` 在 `.cpp` 中。
⚠️ `AMEEImage.cpp` 使用了 `std::memcpy` 但缺少 `#include <cstring>`，需补上。

### 2.1 AMEEImage.hpp

```cpp
// Render/Texture/AMEEImage.hpp
#ifndef __AMEE_IMAGE_H__
#define __AMEE_IMAGE_H__
#pragma once
#include <string>
#include <vector>
#include <cstdint>

namespace AMEE {

struct ImageData {
    std::vector<uint8_t> Pixels;   // RGBA 像素数据
    int Width  = 0;
    int Height = 0;
    int Channels = 4;              // 载入时强制转为 RGBA(4)
};

// CPU 侧图片加载/释放
// 调用 stb_image 解码，输出 RGBA 格式
ImageData LoadImage(const std::string& FilePath);

} // namespace AMEE
#endif // __AMEE_IMAGE_H__
```

### 2.2 AMEEImage.cpp

```cpp
// Render/Texture/AMEEImage.cpp
#include "AMEEImage.hpp"

// stb_image: 必须在 .cpp 中 #define 后 #include
#define STB_IMAGE_IMPLEMENTATION
#include "ThirdParty/stb/stb_image.h"

#include "Core/Log/AMEELog.hpp"

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
```

---

## ✅ 步骤 3：创建 Texture2D 封装（已完成）

### 3.1 AMEETexture2D.hpp

```cpp
// Render/Texture/AMEETexture2D.hpp
#ifndef __AMEE_TEXTURE2D_H__
#define __AMEE_TEXTURE2D_H__
#pragma once
#include "../AMEERHI.hpp"
#include "AMEEImage.hpp"
#include <cstdint>
#include <memory>

namespace AMEE {

class RHI;

class Texture2D {
public:
    Texture2D() = default;
    ~Texture2D();

    // 从文件加载并上传 GPU
    bool Load(RHI* rhi, const std::string& filePath);

    // 从 CPU 数据上传 GPU
    bool Create(RHI* rhi, const ImageData& image,
                RHIFormat format = RHIFormat::RGBA8);

    // 绑定到纹理单元
    void Bind(uint32_t slot = 0) const;

    // 设置采样参数
    void SetFilter(RHIFilter minFilter, RHIFilter magFilter);
    void SetWrap(RHIWrap wrapS, RHIWrap wrapT);

    // 销毁
    void Destroy();

    // Accessors
    uint32_t GetGLTexture() const { return m_TextureID; }
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
```

### 3.2 AMEETexture2D.cpp

```cpp
// Render/Texture/AMEETexture2D.cpp
#include "AMEETexture2D.hpp"
#include "../AMEERHI.hpp"
#include "Core/Log/AMEELog.hpp"

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
```

---

## ❌ 步骤 4：修改 Demo 验证（待完成）

引擎层代码已全部就绪，DemoApp 尚未更新来展示纹理。需要：
1. `DemoApp.hpp` — 添加 `Texture2D` 成员和四边形 `Mesh`
2. `DemoApp.cpp` — `OnInit` 中加载纹理、创建索引四边形；`OnRender` 中绑定纹理、绘四边形
3. Shader 从纯色改为 texcoord + sampler2D（见步骤 5）

### 4.1 修改 DemoApp.hpp

新增成员：

```cpp
// DemoApp.hpp
#include "AMEEFramework/Render/Texture/AMEETexture2D.hpp"

// 在 private 成员中新增：
std::unique_ptr<Texture2D> m_pTexture;
std::unique_ptr<Mesh> m_pQuad;   // 四边形（4 顶点 + 6 索引）
```

### 4.2 修改 DemoApp.cpp

修改 `OnInit`：

```cpp
bool DemoApp::OnInit()
{
    // ... 保留原有 shader 编译代码 ...

    // ---- 新纹理部分 ----
    // 加载纹理
    const std::string texPath = GetResourcePath("Assets/Textures/checkboard.png");
    m_pTexture = std::make_unique<Texture2D>();
    if (!m_pTexture->Load(GetRHI(), texPath)) {
        AMEE_LOG_ERROR("DemoApp", "Failed to load texture");
        return false;
    }

    // 创建四边形（带 UV 坐标）：位置(3) + UV(2) = stride 5
    float quadVertices[] = {
        // position (x,y,z)      // texcoord (u,v)
        -0.8f, -0.8f, 0.0f,      0.0f, 0.0f,
         0.8f, -0.8f, 0.0f,      1.0f, 0.0f,
         0.8f,  0.8f, 0.0f,      1.0f, 1.0f,
        -0.8f,  0.8f, 0.0f,      0.0f, 1.0f,
    };
    uint32_t quadIndices[] = { 0, 1, 2, 0, 2, 3 };  // 2 个三角形

    VertexLayout quadLayout;
    quadLayout.Add(0, 3, RHIDataType::Float)    // position
              .Add(2, 2, RHIDataType::Float);   // texcoord (location=2)

    m_pQuad = std::make_unique<Mesh>();
    if (!m_pQuad->CreateIndexed(GetRHI(), quadVertices, 4, quadIndices, 6, quadLayout)) {
        AMEE_LOG_ERROR("DemoApp", "Failed to create quad mesh");
        return false;
    }

    // 可选：删除旧的三角形 Mesh
    m_pTriangle.reset();

    AMEE_LOG_INFO("DemoApp", "Demo initialized (textured quad)");
    return true;
}
```

修改 `OnRender`：

```cpp
void DemoApp::OnRender(float deltaTime, float totalTime)
{
    RHI* rhi = GetRHI();

    rhi->setClearColor(0.15f, 0.15f, 0.2f, 1.0f);
    rhi->clear();

    // 绑定纹理
    m_pTexture->Bind(0);

    float t = totalTime;
    Mat4 model = Mat4::RotateY(t * 45.0f);

    int w, h;
    GetGLContext()->getSize(w, h);
    float aspect = (float)w / (float)h;

    Mat4 view = Mat4::LookAt({0.0f, 0.0f, 2.5f},
                              {0.0f, 0.0f, 0.0f},
                              {0.0f, 1.0f, 0.0f});
    Mat4 proj = Mat4::Perspective(45.0f, aspect, 0.1f, 100.0f);
    Mat4 mvp = proj * view * model;

    m_pShader->use();
    m_pShader->setMat4("uMVP", mvp.Data());
    m_pShader->setInt("uTexture", 0);   // 纹理单元 0

    m_pQuad->Draw();
}
```

---

## 步骤 5：Shader 修改

Demo 的 fragment shader 需要改成采样纹理：

```glsl
const char* fs = R"(
    #version 410 core
    in vec2 vTexCoord;
    uniform sampler2D uTexture;
    out vec4 fragColor;
    void main() {
        fragColor = texture(uTexture, vTexCoord);
    }
)";
```

Vertex shader 需要输出 texcoord：

```glsl
const char* vs = R"(
    #version 410 core
    layout(location = 0) in vec3 aPos;
    layout(location = 2) in vec2 aTexCoord;
    uniform mat4 uMVP;
    out vec2 vTexCoord;
    void main() {
        gl_Position = uMVP * vec4(aPos, 1.0);
        vTexCoord = aTexCoord;
    }
)";
```

---

## ✅ 步骤 4b：Mesh 支持索引缓冲（已完成）

实际按 4、5、6 顺序开发，但索引缓冲（EBO + `CreateIndexed` + `drawElements`）随步骤 1 一并完成。

`Mesh` 需要新增 `CreateIndexed` 方法：

```cpp
// AMEEMesh.hpp - 新增
bool CreateIndexed(RHI* rhi, const float* vertices, uint32_t vertexCount,
                    const uint32_t* indices, uint32_t indexCount,
                    const VertexLayout& layout, RHIPrimitive primitive = RHIPrimitive::Triangles);
```

`AMEEMesh.cpp` 实现：

```cpp
bool Mesh::CreateIndexed(RHI* rhi, const float* vertices, uint32_t vertexCount,
                          const uint32_t* indices, uint32_t indexCount,
                          const VertexLayout& layout, RHIPrimitive primitive)
{
    // ... 与 Create 基本相同，但额外：
    // 1. 创建 VBO（vertices, stride * vertexCount）
    // 2. 创建 EBO: glGenBuffers → GL_ELEMENT_ARRAY_BUFFER → glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexCount * sizeof(uint32_t), indices, GL_STATIC_DRAW)
    // 3. VAO 绑定 VBO + EBO + layout attributes
}

void Mesh::Draw() const
{
    // if (m_IndexCount > 0)
    //     rhi->drawElements(...);  // RHI 需要新增 drawElements
    // else
    //     rhi->drawArrays(...);
}
```

**⚠️ 注意**：`RHI` 接口也需要新增 `drawElements`：

```cpp
// AMEERHI.hpp - RHI class 新增
virtual void drawElements(RHIPrimitive primitive, uint32_t count, uint32_t offset = 0) = 0;

// AMEERHIOpenGL.mm 实现
void RHIOpenGL::drawElements(RHIPrimitive primitive, uint32_t count, uint32_t offset)
{
    GLenum glPrimitive = /* ... */;
    glDrawElements(glPrimitive, count, GL_UNSIGNED_INT, (const void*)(uintptr_t)(offset));
}
```

Mesh 新增成员：

```cpp
// private 新增
uint32_t m_IndexCount = 0;
uint32_t m_EBO = 0;  // Index buffer (EBO)
```

---

## 7. 纹理寻址 vs 无纹理

无需纹理的纯色渲染可以直接使用现有的 Mesh（顶点着色颜色属性）。纹理系统与此并行存在，不冲突。

---

## 代码规范要求

| 项 | 规范 |
|----|------|
| 命名空间 | `AMEE` |
| 文件名 | `ALEEImage.hpp` / `AMEETexture2D.hpp` |
| Include guard | `__AMEE_IMAGE_H__` / `__AMEE_TEXTURE2D_H__` |
| 成员变量 | `m_` 前缀，PascalCase，指针加 `p`：`m_TextureID` / `m_pRHI` |
| 函数参数 | PascalCase：`int Width, int Height` |
| 访问器 | `GetWidth()` / `GetHeight()` |

注意：`AMEEImage.cpp` 中必须 `#define STB_IMAGE_IMPLEMENTATION` 且只能定义一次（在 .cpp 中）。

---

## 验收标准

| 标准 | 当前状态 |
|------|---------|
| 1. **构建成功**：`BUILD SUCCEEDED` | ✅ 已通过 |
| 2. **窗口显示**：带纹理的旋转四边形 | ❌ DemoApp 待更新 |
| 3. **无纹理时回退**：不崩溃 | ❌ DemoApp 待更新 |
| 4. **日志正确**：`Loaded image` / `Created texture` | ❌ DemoApp 待更新 |
