# AMEEngine

跨平台游戏引擎内核，Objective-C/C++ 编写，OpenGL 4.1。

## 当前状态

```
构建   ✅ BUILD SUCCEEDED
架构   ✅ Application + 平台工厂 + ECS (Entity/Component/Scene)
渲染   ✅ RHI 抽象 + OpenGL 4.1 Core + Mesh/Shader/Texture
资源   ✅ AssetManager (Handle 模式) + FileSystem (Bundle/CWD/Env)
数学   ✅ Vec2/3/4 + Mat4 (旋转/投影/视图/TRS) + Camera
输入   ✅ IPlatformInput + KeyCode/MouseButton + macOS NSEvent
日志   ✅ 分级输出 + 非阻塞 flush
模型   ✅ PrimitiveMesh (Cube/Sphere/Plane/Cylinder/Capsule)
模型   ❌ OBJ/FBX 加载器
音效   ❌
```

## 项目结构

```
engine/
  CODE_CONVENTIONS.md
  Demo/
    AMEEFramework/
      Core/
        AMEEApplication.hpp/cpp       # Application 基类
        AMEEObject.hpp/cpp            # Object (ID + Name)
        AMEENode.hpp/cpp              # Node (层级树)
        AMEECamera.hpp/cpp            # Camera Component
        AMEEScene.hpp/cpp             # Scene (Entity 管理)
        Asset/                        # FileSystem + AssetManager
        Components/                   # Component / MeshFilter / MeshRenderer
        Entity/                       # Entity (Transform + Components)
        Meshes/                       # PrimitiveMesh (Cube/Sphere...)
        Math/                         # Vec2/3/4 + Mat4
        Log/                          # Logger
        Platform/                     # Window / GLContext / Loop / Input 接口
      Platform/macOS/                 # macOS 实现
        GL/                           # RHIOpenGL (OpenGL 4.1 Core)
      Render/
        AMEERHI.hpp                   # RHI 抽象接口
        AMEEMesh.hpp/cpp              # VBO/VAO/EBO
        AMEEVertexLayout.hpp          # 顶点布局
        Shader/                       # ShaderProgram + GL 后端
        Texture/                      # Texture2D + ImageData
    Sources/
      DemoApp.hpp/cpp                 # Demo 应用
      AMEEAppController.mm            # ObjC 桥接
    Assets/
      Shaders/                        # Default.vert / Default.frag
      Textures/                       # 03.png / 04.png
    docs/specs/                       # 功能开发规格书
    Demo.xcodeproj
  README.md
  LICENSE
```

## 架构概览

```
Application
  ├── FileSystem      # 虚拟文件系统 (Bundle/CWD/ENV)
  ├── Window          # 平台窗口
  ├── GLContext       # OpenGL 上下文
  ├── RHI             # 渲染硬件接口
  ├── GameLoop        # OnFixedUpdate (60Hz) + OnRender (可变)
  └── Input           # 键盘/鼠标

Scene : Node, Object
  └── Entity("Cube") : Node, Object
  │     ├── MeshFilter     → MeshHandle
  │     └── MeshRenderer   → ShaderHandle + TextureHandle
  │                          Draw(ViewProj)
  └── Entity("Camera")
        └── Camera Component (Yaw/Pitch/FOV)
             GetViewMatrix() → 从 Owner 取 Position
```

## 代码规范

- **命名空间**：`AMEE`
- **变量**：`m_` / `m_p` / `g_` / `gs_` + PascalCase
- **Include guard**：`__AMEE_文件名大写_H__`
- 详见 `CODE_CONVENTIONS.md`

## 构建

```bash
cd Demo
xcodebuild -project Demo.xcodeproj -scheme Demo -configuration Debug build
```

开发时需设环境变量：`AMEE_ASSETS_PATH` → 项目 Demo 目录。

## 下阶段开发计划

### 🔴 P0 — 近期

| 功能 | 说明 | 文档 |
|------|------|------|
| OBJ 模型加载 | Phase 1: 静态顶点/UV/法线，Phase 2: MTL 材质 | [spec](./Demo/docs/specs/AMEE_Model_System.md) |
| FBX 模型加载 | ufbx 集成，支持骨骼动画 | [spec](./Demo/docs/specs/AMEE_Model_System.md) |
| 简单光照 | Phong per-pixel (ambient + diffuse + specular) | — |
| RenderPass/Framebuffer | RHI 加 createFramebuffer，后处理基础 | — |

### 🟡 P1 — 中期

| 功能 | 说明 |
|------|------|
| ImGui 集成 | 调试 UI / 属性面板 |
| UBO 支持 | Uniform Buffer Object，批量 Uniform 更新 |
| 粒子系统 | 技能特效 |
| 音效 | OpenAL 或 AVAudioEngine，stb_vorbis 已集成 |

### 🟢 P2 — 远期

| 功能 | 说明 |
|------|------|
| UI 系统 | 菜单/HUD |
| 物理 | AABB 碰撞 |
| 网络层 | TCP/UDP + 数据包 |
| Metal 后端 | macOS 长期方向 |
| 编辑器 | 可视化工具 |

## 文档

- [模型系统规格](./Demo/docs/specs/AMEE_Model_System.md)
- [纹理系统规格](./Demo/docs/specs/AMEE_Texture_System.md)
- [代码规范](./CODE_CONVENTIONS.md)

## License

MIT © 2026
