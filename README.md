# AMEEngine

跨平台游戏引擎内核，Objective-C/C++ 编写，OpenGL 4.1。

## 当前状态

架构基础已定型，进入细节打磨阶段。

```
构建   ✅ BUILD SUCCEEDED
架构   ✅ Application 抽象层 + 平台工厂 + 代码规范全合规
渲染   ✅ RHI 抽象 + OpenGL 4.1 后端 + Mesh/Shader 管线
数学   ✅ Vec2/3/4 + Mat4（旋转/投影/视图/TRS）
日志   ✅ 分级输出 + 颜色 + 文件
纹理   🔄 规格文档已完成，待开发（stb_image 已集成）
输入   ❌
场景   ❌
音效   ❌
```

## 项目结构

```
engine/
  CODE_CONVENTIONS.md         # 代码规范（命名/命名空间/guard/变量前缀）
  Demo/
    AMEEFramework/             # 引擎框架
      Core/                    # 平台无关核心
        AMEEApplication.hpp    # Application 基类（工厂方法 + Init/Run/Shutdown）
        Math/                  # 数学库
        Log/                   # 日志系统
        Platform/              # 平台抽象接口
      Render/                  # 渲染抽象
        AMEEMesh.hpp           # Mesh 一站式管理（VAO/VBO + EBO）
        AMEEVertexLayout.hpp   # 声明式顶点布局
        Shader/                # Shader 系统
          GL/                  # OpenGL 后端
        Texture/               # 纹理系统（即将开发）
      Platform/                # 平台实现
        macOS/                 # macOS 实现
          AMEEMacosApplication.hpp  # 工厂方法
      ThirdParty/              # 第三方库
        stb/                   # stb_image.h / stb_image_write.h / stb_truetype.h
    Sources/                   # 用户应用
      DemoApp.hpp/cpp          # 继承 MacosApplication，业务逻辑
      AMEEAppController.mm     # ObjC 桥接层（仅菜单+生命周期）
    Demo/                      # Xcode 项目
    docs/
      specs/                   # 功能开发规格书
        AMEE_Texture_System.md # 纹理系统规格
  README.md
  LICENSE
```

## 架构概览

```
AMEEAppController (ObjC 桥接, 菜单)
  └─> DemoApp : MacosApplication
        ├─ OnInit()    → 创建 Shader, Mesh, Texture
        ├─ OnRender()  → 每帧渲染
        └─ OnShutdown()→ 资源清理

AMEE::Application (基类)
  ├─ Init()    → 创建 Window/GLContext/RHI
  ├─ Run()     → 启动 GameLoop
  └─ Shutdown()→ 清理
  工厂方法 (由 MacosApplication 实现):
    CreatePlatformWindow / CreatePlatformGLContext / CreatePlatformGameLoop / CreateRHI
```

## 代码规范

- **命名空间**：`AMEE`（两个大写 E）
- **变量**：`m_` / `m_p` / `g_` / `gs_` + PascalCase
- **Include guard**：`__AMEE_去掉AMEE前缀_H__`
- **文件**：C++ 头 `.hpp`，ObjC 头 `.h`，C++ 源码 `.cpp`，ObjC++ 源码 `.mm`
- 详细规范见 `CODE_CONVENTIONS.md`

## 构建

```bash
cd Demo
xcodebuild -project Demo.xcodeproj -scheme Demo -configuration Debug build
```

或在 Xcode 中打开 `Demo.xcodeproj` → Cmd+R。

## 开发路线

| 阶段 | 功能 | 状态 |
|------|------|------|
| 基础架构 | Application/平台工厂/Mesh | ✅ |
| 渲染管线 | RHI/Shader/纹理 | 🔄 纹理待开发 |
| 资源管理 | AssetManager/模型加载 | ❌ |
| 场景系统 | Entity/Scene/Input | ❌ |
| 游戏 Demo | MuOnline 原型 | ❌ |

## 文档

- [开发说明](./Demo/README_DEV.md)
- [纹理系统规格 (AI 可用)](./Demo/docs/specs/AMEE_Texture_System.md)
- [代码规范](./CODE_CONVENTIONS.md)

## License

MIT © 2026
