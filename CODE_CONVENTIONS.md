# AMEonEngine 代码规范

> AMEonEngine 游戏引擎专项规范，不适用于其它项目。
> 需要时手动加载，不需要时忽略。

## 一、命名空间规范

### 1.1 顶级命名空间

- 所有代码必须使用 `AMEE` 作为顶级命名空间，`AMEE`连续两个E，并且都是大写
- 禁止使用 `Amee`、`amee`、`AMee` 等其他变体

### 1.2 层级结构

- 命名空间层级使用 `::` 分隔
- 子命名空间必须位于 `AMEE` 之下
- 示例：
  ```cpp
  namespace AMEE { }
  namespace AMEE::Core { }
  namespace AMEE::Game { }
  namespace AMEE::Game::Renderer { }
  namespace AMEE::Core::Math { }
  ```

### 1.3 禁止事项

- 禁止在 `AMEE` 命名空间之外定义新的全局命名空间
- 禁止嵌套过深的命名空间（建议不超过 4 层）

### 1.4 平台名
- Macos：Macos 不得使用Mac，MacOS，macOS，macos
- Windows：Windows 不得使用Win，win，windows
- Linux：Linux 不得使用linux

## 二、文件名规范

### 2.1 文件名与命名空间对应

- 文件名由命名空间各层级 + 类名拼接而成，类名本身不需要重复命名空间前缀
- 所有部分均使用大写 PascalCase（首字母大写）
- 如果文件不在模块目录中需要命名空间各层级直接拼接，不加分隔符，如 `AMEECoreLog.hpp`
- 如果文件在模块中，则可以简化，如 `Renderer\AMEERenderer.hpp`
- 接口文件命名必须I开头，然后再接文件名，如 `IAMEEGameRenderer.hpp`，不得把I写在命名中
- 示例：
  | 完整类名 | 文件名 |
  |---------|--------|
  | `AMEE::Game::Renderer` | `AMEEGameRenderer.hpp` |
  | `AMEE::Core::Math::Vector3` | `AMEECoreMathVector3.hpp` |
  | `AMEE::Game::Entity` | `AMEEGameEntity.hpp` |
  | `AMEE::Core::Log` | `AMEECoreLog.hpp` |

### 2.2 文件扩展名

- C 头文件：`.h`
- C++ 头文件：`.hpp`
- C 源文件：`.c`
- C++ 源文件：`.cpp`
- Objective-C++ 源文件：`.mm`

### 2.3 目录结构

- 目录结构与命名空间层级对应
- 建议按模块分组，不强制一对一映射
- 示例目录结构：
  ```
  AMEE/
  ├── Core/
  │   ├── AMEECoreMathVector3.hpp
  │   ├── AMEECoreMathVector3.cpp
  │   └── AMEECoreLog.hpp
  ├── Game/
  │   ├── AMEEGameRenderer.hpp
  │   ├── AMEEGameRenderer.cpp
  │   └── AMEEGameEntity.hpp
  ```

### 2.4 禁止事项

- 禁止文件名使用小写、下划线或短横线
- 禁止文件名与实际类名不匹配
- 禁止混合命名风格（如 `AmeeGame_Renderer.hpp`）


## 三、头文件保护宏规范

### 3.1 命名格式

- 所有 `.h` 和 `.hpp` 文件必须包含头文件保护宏
- 格式：`__AMEE_文件名大写_H__`，hpp 文件可选`__AMEE_文件名大写_HPP__`
- 文件名中的 `.` 替换为 `_`，整体大写，且不包含 AMEE 前缀
- 示例：
  | 文件名 | 保护宏 |
  |--------|--------|
  | `AMEEGameRenderer.hpp` | `__AMEE_GAMERENDERER_H__` |
  | `AMEECoreMathVector3.hpp` | `__AMEE_COREMATHVECTOR3_H__` |
  | `AMEECoreLog.h` | `__AMEE_CORELOG_H__` |

### 3.2 格式要求

```cpp
// 正确
#ifndef __AMEE_GAMERENDERER_H__
#define __AMEE_GAMERENDERER_H__
// ... 内容 ...
#endif // __AMEE_GAMERENDERER_H__

// #pragma once 可选，但不能替代 #ifndef（Windows 平台必须有保护宏）
// #pragma once  // 可选
```

```cpp
// 错误
#ifndef __AMEE_AMEEGAMERENDERER_H__
#define __AMEE_AMEEGAMERENDERER_H__
// ... 内容 ...
#endif // __AMEE_AMEEGAMERENDERER_H__
```

### 3.3 禁止事项

- 禁止省略 `#endif` 后的注释
- 禁止多个头文件使用相同的保护宏


## 四、变量命名规范

### 4.1 前缀规则

| 变量类型 | 前缀 | 示例 |
|---------|------|------|
| 全局变量 | `g_` | `g_RunningInstance`、`g_ModuleCount` |
| 类成员变量 | `m_` | `m_Position`、`m_EntityList` |
| 静态成员变量 | `ms_` | `ms_InstanceCount`、`ms_CachedData` |
| 静态全局变量 | `gs_` | `gs_SingletonPtr`、`gs_GlobalConfig` |
| 局部变量 | 无前缀，PascalCase | `RunningSpeed`、`FrameCount` |
| 函数参数 | 无前缀，PascalCase | `TargetPosition`、`InputData` |
| 指针 | 必须增加 `p` 前缀 | `pRunningSpeed`、`pFrameCount` |
| 向量类 | 必须增加 `v` 前缀  | `vPosition`、`vRotation`、`vScale` |

### 4.1.1 数学库例外

`Vec2`/`Vec3`/`Vec4`/`Mat4` 等基础数学类型的公开成员字段（`x`、`y`、`z`、`w`、`m[16]`）豁免 `v` 前缀和 PascalCase 要求，保持小写单字母形式，以符合数学领域惯例和 GLSL 兼容性：

```cpp
// 数学类型 - 成员字段豁免
struct Vec3 {
    float x, y, z;          // ✅ 豁免：不需要 vX, vY, vZ
    static Vec3 Cross(...); // ✅ 函数名仍需 PascalCase
};

// 非数学类型 - 不豁免
class Entity {
    Vec3 m_vPosition;       // ✅ 成员变量仍需 v 前缀
};
```

- **豁免范围**：仅限 `Vec2/Vec3/Vec4/Mat4` 结构体的公开字段（`x, y, z, w, m`）
- **不豁免**：数学类型的函数名、函数参数、局部变量仍需遵循 PascalCase 规范
- **不豁免**：其他类中类型为 `Vec*` 的成员变量仍需 `m_v` 前缀

示例：
```cpp
namespace AMEE::Game {
  // 全局指针
  AMEE::Game::Renderer* g_pRunningInstance = nullptr;
  class Entity {
  private:
    // 类成员指针
    AMEE::Game::Renderer* m_pRunningInstance = nullptr;
    //
    Vector3 m_vPosition;
  };
}


### 4.2 命名风格

- 所有变量使用 PascalCase（首字母大写），如 `GameState`、`EntityCount`
- 禁止使用 `snake_case`、`camelCase`（非 AMEE 风格）
- 变量名应具有描述性，禁止使用单字母（循环计数器除外，如 `i`、`j`、`k`）

### 4.3 示例

```cpp
namespace AMEE::Game {

    // 全局变量
    int g_ModuleCount = 0;
    Renderer* g_pRunningInstance = nullptr;

    class Entity {
    public:
        void Update(float DeltaTime);

    private:
        // 类成员变量
        Vector3 m_vPosition;
        int m_EntityId;
        static int ms_InstanceCount;  // 静态成员变量
    };
}
```

### 4.4 禁止事项

- 禁止全局变量暴露在头文件中（头文件禁止定义非 `inline` 的非 `const` 变量）
- 禁止使用裸 `int`、`float` 等基础类型作为全局状态
- 禁止变量名与标准库或系统名称冲突
