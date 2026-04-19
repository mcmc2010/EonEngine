# AMEEFramework 开发规范

> 详细代码规范参见项目根目录 `CODE_CONVENTIONS.md`

## 项目结构

```
AMEEFramework/           # 引擎框架 (独立 target)
  Core/                 # 核心模块 (平台无关)
    AMEEApplication.hpp # Application 抽象基类
    Math/               # 数学库 (Vec2/Vec3/Vec4/Mat4)
    Log/                # 日志系统
    Platform/           # 平台抽象接口
  Render/               # 渲染抽象层
    AMEEMesh.hpp        # Mesh 抽象 (VAO/VBO 管理)
    AMEEVertexLayout.hpp # 顶点布局声明式描述
    Shader/             # Shader 系统
      GL/                # OpenGL 后端
  Platform/             # 平台实现
    macOS/              # macOS 实现
      AMEEMacosApplication.hpp  # macOS Application 工厂
  AMEEFramework.h/mm    # 框架 ObjC 入口

Sources/                 # 用户应用
  DemoApp.hpp/cpp       # 继承 MacosApplication，实现业务逻辑
  AMEEAppController.mm  # ObjC 桥接层 (仅菜单+生命周期)

Demo/                    # Xcode 项目
  main.mm               # 入口点
```

## 架构概览

```
┌─────────────────────────────────────────────┐
│  AMEEAppController (ObjC 桥接, 菜单)         │
│    └─> DemoApp : MacosApplication           │
│          ├─ OnInit()    → 创建 Shader, Mesh  │
│          ├─ OnRender()  → 每帧渲染           │
│          └─ OnShutdown()→ 资源清理           │
├─────────────────────────────────────────────┤
│  AMEE::Application (基类)                    │
│    ├─ Init()    → 创建 Window/GLContext/RHI  │
│    ├─ Run()     → 启动 GameLoop              │
│    └─ Shutdown()→ 清理所有平台资源           │
│    工厂方法 (由 MacosApplication 实现):       │
│      CreatePlatformWindow()                  │
│      CreatePlatformGLContext()               │
│      CreatePlatformGameLoop()                │
│      CreateRHI()                             │
├─────────────────────────────────────────────┤
│  AMEE::Mesh + VertexLayout                  │
│    Mesh.Create(rhi, vertices, layout)        │
│    Mesh.Draw()                               │
│    VertexLayout.Add(index, count, type)...   │
└─────────────────────────────────────────────┘
```

## 命名规则

### 命名空间

所有引擎代码使用 `AMEE` 命名空间（两个大写 E）：

```cpp
namespace AMEE {
class MacosWindow : public IPlatformWindow { ... };
}
```

### 文件名

| 类型 | 规则 | 示例 |
|------|------|------|
| C++ 头文件 | `.hpp`，AMEE前缀+PascalCase | `AMEEMath.hpp`, `AMEEApplication.hpp` |
| ObjC 头文件 | `.h` | `AMEEFramework.h`, `AMEEAppController.h` |
| 接口文件 | I开头在AMEE前 | `IAMEEPlatformWindow.hpp` |
| C++ 源文件 | `.cpp` | `AMEEMesh.cpp`, `AMEEApplication.cpp` |
| ObjC++ 源文件 | `.mm` | `AMEEMacosWindow.mm` |

### 类名

| 类型 | 规则 | 示例 |
|------|------|------|
| 抽象接口 | I 前缀 | `IPlatformWindow`, `IPlatformGLContext` |
| 平台实现 | Macos 前缀 | `MacosWindow`, `MacosGLContext`, `MacosGameLoop` |
| 渲染抽象 | 无前缀 | `RHI`, `ShaderProgram`, `Mesh` |
| 渲染后端 | GL 前缀 | `RHIOpenGL`, `GLShaderProgram` |

### 变量命名

| 类型 | 前缀 | 示例 |
|------|------|------|
| 成员变量 | `m_` | `m_pWindow`, `m_Running` |
| 指针成员 | `m_p` | `m_pRHI`, `m_pGlView` |
| 全局变量 | `g_` | `g_pAmeeFramework`, `g_MinLevel` |
| 静态成员 | `ms_` | `ms_InstanceCount` |
| 函数/参数/局部 | PascalCase | `CreateRHI()`, `DeltaTime`, `FrameCount` |

### Include Guard

格式 `__AMEE_去掉AMEE前缀的文件名大写_H__`：

```cpp
// AMEEMath.hpp → __AMEE_MATH_H__
// IAMEEPlatformWindow.hpp → __AMEE_IPLATFORMWINDOW_H__
// AMEERHI.hpp → __AMEE_RHI_H__
```

### 数学库例外

`Vec2/Vec3/Vec4/Mat4` 的公开字段（`x, y, z, w, m[16]`）豁免 `v` 前缀和 PascalCase，但函数名和其他类的 Vec 成员变量不豁免。

## 代码组织原则

1. **平台无关代码** → `Core/`, `Render/`
   - Application, Math, Log, RHI, Mesh, Shader 等
   - 可跨平台编译

2. **平台相关代码** → `Platform/`
   - 接口定义在 `Core/Platform/`
   - 实现放在 `Platform/macOS/`, `Platform/Windows/`, 等
   - 工厂方法在 `MacosApplication` 中

3. **用户代码** → `Sources/`
   - 继承 `MacosApplication`，只关心业务逻辑
   - 通过 `OnInit/OnRender/OnShutdown` 钩子接入

## 示例

### 创建新的 Demo 应用

```cpp
// MyGame.hpp
#include "AMEEFramework/Platform/macOS/AMEEMacosApplication.hpp"
#include "AMEEFramework/Render/AMEEMesh.hpp"

namespace AMEE {
class MyGame : public MacosApplication {
protected:
    bool OnInit() override;
    void OnRender(float DeltaTime, float TotalTime) override;
    void OnShutdown() override;
private:
    std::unique_ptr<Mesh> m_pMesh;
};
}
```

### 使用 Mesh + VertexLayout

```cpp
// 之前: 手动管理 VAO/VBO
m_Rhi->bindVertexArray(vao);
m_Rhi->bindVertexBuffer(vbo);
m_Rhi->enableVertexAttribArray(0);
m_Rhi->vertexAttribPointer(0, 3, RHIDataType::Float, false, stride, (void*)0);

// 现在: 声明式布局
VertexLayout layout;
layout.Add(0, 3, RHIDataType::Float)    // position
      .Add(1, 3, RHIDataType::Float);   // color

m_pMesh = std::make_unique<Mesh>();
m_pMesh->Create(GetRHI(), vertices, vertexCount, layout);

// 渲染
m_pMesh->Draw();
```

### 添加新平台

```
Platform/windows/
  AMEEWindowsApplication.hpp   # 继承 Application，实现工厂方法
  AMEEWindowsApplication.cpp
  AMEEWindowsWindow.hpp/cpp    # 实现 IPlatformWindow
  AMEEWindowsGLContext.hpp/cpp # 实现 IPlatformGLContext
  AMEEWindowsGameLoop.hpp/cpp  # 实现 IPlatformLoop
```

## 编译目标

- `AMEEFramework.framework` - 引擎框架
- `Demo.app` - 用户应用
