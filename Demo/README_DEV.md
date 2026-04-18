# AMEEFramework 开发规范

## 项目结构

```
AMEEFramework/           # 引擎框架 (独立 target)
  Core/                 # 核心模块 (平台无关)
    Math/               # 数学库
    Log/                # 日志系统
    Platform/           # 抽象接口
  Render/               # 渲染抽象层
    Shader/             # Shader 系统
      GL/                # OpenGL 后端
  Platform/             # 平台实现
    macOS/              # macOS 实现
  AMEEFramework.h/mm   # 框架入口

Demo/                   # 用户应用 (链接引擎)
  Sources/              # 应用代码
  main.mm               # 入口点
```

## 命名规则

### 文件名

| 类型 | 规则 | 示例 |
|------|------|------|
| 公共类/接口 | AMEE 前缀 | `AMEEMath.h`, `AMEELog.h` |
| 内部使用 | 无需前缀 | `IPlatformWindow.h` |

### 类名

| 类型 | 规则 | 示例 |
|------|------|------|
| 引擎核心类 | 普通 (无命名空间) | `class Logger` |
| 抽象接口 | I 前缀 | `IPlatformWindow` |
| 平台实现 | Mac 前缀 | `MacWindow`, `MacGameLoop` |

### 代码组织原则

1. **平台无关代码** → `Core/`, `Render/`
   - Math, Log, RHI, Shader 等
   - 可跨平台编译

2. **平台相关代码** → `Platform/`
   - 接口定义在 `Core/Platform/`
   - 实现放在 `Platform/macOS/`, `Platform/linux/`, 等

3. **用户代码** → `Demo/Sources/`
   - 仅引用引擎，不涉及引擎内部

## 命名空间

**不使用命名空间**，通过文件名前缀区分命名空间。

```cpp
// 引擎代码
class Logger { };

// 用户代码
amee::Logger logger;  // 不需要
Logger logger;        // 直接使用
```

## 禁止事项

1. **禁止**在引擎代码中使用 `using namespace`
2. **禁止**在头文件中 `using namespace`
3. **禁止**在全局命名空间中添加可能冲突的符号

## 示例

### 添加新模块

```
Core/Log/
  AMEELog.h      # 公共接口 (AMEE 前缀)
  AMEELog.cpp   # 实现
```

### 添加新平台

```
Platform/linux/
  LinuxWindow.h     # 实现 IPlatformWindow
  LinuxWindow.cpp
```

### 用户使用引擎

```cpp
#include <AMEEMath/AMEEMath.h>

void example() {
    AMEE::Mat4 mat = AMEE::Mat4::identity();
}
```

## 编译目标

- `AMEEFramework.framework` - 引擎框架
- `Demo.app` - 用户应用