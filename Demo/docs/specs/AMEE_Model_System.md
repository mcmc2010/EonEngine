# 模型加载系统 — 开发规格

## 目标

支持两种模型格式：
- **OBJ** — 轻量、纯文本、无动画，适合静态场景道具
- **FBX** — 复杂、二进制、支持骨骼动画，适合角色

---

## OBJ 加载器

### Phase 1 — 最小可用（无材质）

| 项目 | 说明 |
|------|------|
| 解析 | `v` `vt` `vn` `f` 四类行，忽略材质/group/smoothing |
| 输出 | `std::vector<float>` 顶点缓冲区 + `std::vector<uint32_t>` 索引缓冲区 |
| Mesh | 组装为一个 `Mesh` 对象，`VertexLayout` 自动生成 |
| 三角化 | `f` 面支持三角形和四边形，四边形拆两个三角形 |
| 错误处理 | 文件不存在、格式错误 → 日志 + 返回空 |

### Phase 2 — 多 Mesh + 材质

| 项目 | 说明 |
|------|------|
| `usemtl` | 按材质分组，一个 .obj 生成多个 Mesh |
| `mtllib` | 解析 .mtl 文件，提取 `map_Kd` 纹理路径 |
| AssetManager | 自动加载关联纹理 |

### Phase 3 — 法线 + 切线

| 项目 | 说明 |
|------|------|
| 法线 | 无 `vn` 时自动生成面法线/顶点法线 |
| 切线 | 有 UV 时计算切线，为法线贴图做准备 |

### 文件结构

```
AMEEFramework/Render/Model/
├── AMEEModelLoader.hpp        # OBJ 解析器声明
├── AMEEModelLoader.cpp        # OBJ 解析实现
├── AMEEModelData.hpp          # 中间数据结构 (顶点+索引+材质组)
└── AMEEMtlLoader.hpp/.cpp     # MTL 材质解析 (Phase 2)
```

### 示例 OBJ

```
# cube.obj
v -0.5 -0.5  0.5
v  0.5 -0.5  0.5
v  0.5  0.5  0.5
v -0.5  0.5  0.5
vt 0 0
vt 1 0
vt 1 1
vt 0 1
f 1/1 2/2 3/3 4/4
```

---

## FBX 加载器

### 方案选择

| 方案 | 优点 | 缺点 |
|------|------|------|
| OpenFBX | 单头文件、MIT、解析骨骼+动画 | 社区小 |
| ufbx | 单头文件、MIT、性能好、维护活跃 | 较新 |
| Assimp | 功能最全 | 库大、构建复杂 |
| 自研 | 无依赖 | FBX 格式极其复杂，不现实 |

**选择：ufbx** — 单头文件嵌入，MIT 协议，API 干净。

### Phase 1 — 静态模型

| 项目 | 说明 |
|------|------|
| 集成 | `ThirdParty/ufbx/ufbx.h` 单头文件 |
| 解析 | 提取顶点位置、UV、法线、索引 |
| Mesh | 组装为 `Mesh` 对象 |
| 多 Mesh | 按 material slot 拆分子 Mesh |

### Phase 2 — 骨骼 + 动画

| 项目 | 说明 |
|------|------|
| 骨架 | 骨骼层级 + bind pose |
| 蒙皮 | 顶点权重 + joint indices |
| 动画 | 关键帧采样 + 插值 |
| Shader | `u_BoneMatrices[64]` uniform array |

### Phase 3 — 混合动画

| 项目 | 说明 |
|------|------|
| 动画混合 | 两个 clip 之间 lerp |
| 动画状态机 | 空闲/行走/攻击 切换 |
| 动画事件 | 脚步、攻击判定帧回调 |

### 文件结构

```
AMEEFramework/Render/Model/
├── AMEEFbxLoader.hpp/.cpp     # FBX 封装，依赖 ufbx
```

---

## AssetManager 集成

```
AssetManager::LoadModel("Models/cube.obj")       → MeshHandle
AssetManager::LoadModel("Models/character.fbx")  → MeshHandle (+ Skeleton)
```

内部根据扩展名分发到 OBJ 或 FBX 加载器，返回统一的 `MeshHandle`。

---

## 开发顺序

```
Phase 1  ████████░░  OBJ 静态加载 (1-2天)
Phase 2  ████░░░░░░  MTL 材质 (0.5天)
Phase 3  ████░░░░░░  法线/切线 (0.5天)
Phase 4  ████████░░  FBX 静态加载 (1-2天)
Phase 5  ██████████  FBX 骨骼 + 动画 (3-5天)
Phase 6  ██████░░░░  动画混合 + 状态机 (2-3天)
```

---

## 前置依赖

- ✅ Mesh (VAO/VBO/EBO)
- ✅ VertexLayout
- ✅ Texture2D
- ✅ AssetManager
- ✅ Camera + Input
