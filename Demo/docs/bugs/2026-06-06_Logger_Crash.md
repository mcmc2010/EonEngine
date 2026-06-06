# Logger 双重崩溃 Bug

日期: 2026-06-06
模块: `Core/Log/AMEELog.cpp`
影响: 应用启动即崩溃，窗口无法显示

## Bug 1: va_list use-after-consume

### 症状
应用启动时崩溃，debugger 停在 `applicationDidFinishLaunching` 的 `Init()` 调用处。

### 原因
`Logger::vlog` 中 `va_list args` 被 `vsnprintf` 消耗后，同一个 `args` 又传给了 `fprintf`（文件日志路径）。ARM64 上 `va_list` 本质是指针，`vsnprintf` 接过后内部游标走到末尾，第二次读取是未定义行为 → 崩溃。

```cpp
// 修复前 — UB：args 被 vsnprintf 消耗后 fprintf 再读是垃圾
char msg[1024];
vsnprintf(msg, sizeof(msg), fmt, args);   // args 消耗
fprintf(stderr, ...);
if (g_pFileLog) {
    fprintf(g_pFileLog, ..., msg);         // 用已消耗的 args → UB
}
```

### 修复
首次使用前 `va_copy` 复制一份，两处各用各的：

```cpp
va_list argsCopy;
va_copy(argsCopy, args);
vsnprintf(msg, sizeof(msg), fmt, argsCopy);
va_end(argsCopy);
```

## Bug 2: std::mutex 重入死锁

### 症状
应用启动后卡死（无响应，不退出）。

### 原因
`Logger::init` 持有 `g_LogMutex` 后调用 `AMEE_LOG_INFO` → `Logger::log` → `Logger::vlog` 再次尝试加锁同一个 `std::mutex`。`std::mutex` 不可重入，同一线程等自己释放 → 死锁。

调用链：
```
Logger::init()        ← lock(g_LogMutex)
  └─ AMEE_LOG_INFO()
       └─ Logger::log()
            └─ Logger::vlog()  ← lock(g_LogMutex) → 死锁
```

### 修复
`std::mutex` → `std::recursive_mutex`，允许同一线程重复加锁。

## 关键教训

- `va_list` 只能用一次，需要多次使用必须 `va_copy`
- `std::mutex` 不可重入，模块内部调用自身公开接口时必须用 `std::recursive_mutex` 或拆分加锁粒度
- Logger 是全局基础设施，启动时最先被调用，崩溃表现往往指向下游模块（如 `Init()`），排查时应从最底层开始
