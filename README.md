# AMEEFramework

A lightweight, cross-platform game engine bootstrap layer written in Objective-C/C++.

## Overview

App + Dynamic Framework architecture (similar to Unity's `UnityPlayer` model):
- **Demo.app** — thin shell that dynamically loads the engine framework at runtime
- **AMEEFramework.framework** — engine core: game loop, render pipeline, platform abstraction

## Current Status

- [x] Programmatic window & menu bar (no Storyboard dependency)
- [x] Game loop (CVDisplayLink driven, variable timestep)
- [x] Time management (deltaTime, timeScale, frame clamping)
- [x] RHI abstraction layer (platform-independent render interface)
- [x] OpenGL 4.1 backend (clear screen verified)
- [ ] Shader pipeline & mesh rendering
- [ ] Input system
- [ ] Asset management
- [ ] Scene / entity / component system
- [ ] Networking
- [ ] Scripting runtime
- [ ] Cross-platform (Linux / Windows / iOS)

## Requirements

- macOS 11.5+
- Xcode 16.4+
- C++20 / Objective-C++

## Build

```bash
cd Demo
xcodebuild -project Demo.xcodeproj -scheme Demo -configuration Debug build
```

## Run

Open `Demo.xcodeproj` in Xcode and press **Cmd+R**.

## Architecture

```
Demo.app (thin shell)
  └── main() → AMEEFrameworkLoad() → runApplicationMain()
                  │
                  └── AMEEFramework.framework
                        ├── AMEEFramework       singleton, bootstrap
                        ├── Core/
                        │   ├── AMEETime        deltaTime, timeScale
                        │   └── AMEEGameLoop     CVDisplayLink → update → render
                        ├── Render/
                        │   ├── RHI.h            abstract interface
                        │   └── GL/
                        │       └── RHIOpenGL    OpenGL 4.1 backend
                        ├── Platform/
                        │   └── macOS/
                        │       └── GLView       NSOpenGLView wrapper
                        └── Sources/
                            └── AMEEAppController window, menu, lifecycle
```

### Render Pipeline

```
engine logic
    ↓
RHI (abstract: setClearColor / clear / setViewport / present)
    ↓
RHIOpenGL (gl* calls, cross-platform)
    ↓
Platform GL Context (macOS: GLView, Linux: GLX, Windows: WGL)
```

Only context creation is platform-specific. Adding Vulkan = one new backend file.

## Project Structure

```
engine/
  Demo/
    AMEEFramework/          # Framework target
      Core/                 # Game loop & time
      Render/               # RHI abstraction
        GL/                 # OpenGL backend
      Platform/
        macOS/              # macOS-specific code
    Sources/                # Framework target (AppController)
    Demo/                   # App target (main.mm entry)
    docs/                   # Architecture design doc
```

## Documentation

- [Architecture Design (Chinese)](Demo/docs/AMEEFramework_Architecture_Design.md)

## License

[MIT](LICENSE) © 2026
