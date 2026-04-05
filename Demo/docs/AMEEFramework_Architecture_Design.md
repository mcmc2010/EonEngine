# AMEEFramework

macOS game engine bootstrap layer. App + Dynamic Framework architecture.

## Build

```
xcodebuild -project Demo.xcodeproj -scheme Demo -configuration Debug build
```

**Stack**: ObjC/ObjC++ (C++20), macOS 11.5+, Xcode 16.4, ARC

## Architecture

```
Demo.app                                          (thin shell, ~3 files)
  main.mm → AMEEFrameworkLoad() → runApplicationMain()
  │
  └── Contents/Frameworks/
        └── AMEEFramework.framework              (engine core)
              ├── AMEEFramework.mm                (singleton, warmup, app bootstrap)
              ├── Core/AMEETime.mm                (deltaTime, timeScale)
              ├── Core/AMEEGameLoop.mm            (CVDisplayLink → update → render)
              ├── Render/RHI.h                    (abstract render interface)
              ├── Render/GL/RHIOpenGL.mm          (OpenGL 4.1 backend)
              ├── Platform/macOS/GLView.mm        (NSOpenGLView, GL context)
              └── Sources/AMEEAppController.mm    (window, menubar, lifecycle)
```

**2 Targets**: `Demo` (app) depends on `AMEEFramework` (framework), embedded via `@rpath`.

## Startup Sequence

```
main()
  AMEEFrameworkLoad()
    bundlePath = mainBundle.bundlePath + "/Contents/Frameworks/AMEEFramework.framework"
    [bundle load]
    NSClassFromString(@"AMEEFramework")          // reliable, not principalClass
    AMEEFramework.getInstance
    setExecuteHeader(&_mh_execute_header)         // Mach-O for dynamic libs
  runApplicationMain()
    frameworkWarmup()
      AMEEFunctionsForDynamicLibs()              // TODO: register dylib funcs
      signal(SIGPIPE, SIG_IGN)                   // prevent socket crash
    [NSApplication sharedApplication]
    app.activationPolicy = Regular
    AMEEAppController *controller = [[AMEEAppController alloc] init]  // MUST create first
    [app setDelegate: controller]
    [app activateIgnoringOtherApps:YES]
    [app run]
      → applicationDidFinishLaunching:
          setupMenuBar()
          setupMainWindow()                        // 1024×768 GLView, code-created
          RHIOpenGL init
          AMEEGameLoop.start                      // CVDisplayLink → main queue
```

No Storyboard. No NIB. Everything is code-created.

## Render Pipeline

### RHI (Render Hardware Interface)

```
gameLoopRender
  ├─ glView.makeGLContextCurrent
  ├─ rhi.setViewport(drawableSize)
  ├─ rhi.setClearColor(r, g, b, a)
  ├─ rhi.clear()                    → glClear(COLOR | DEPTH)
  └─ rhi.present()                  → [NSOpenGLContext flushBuffer]
```

### Layer Design

```
┌─────────────────────────────────┐
│  gameLoopRender (engine logic)   │  ← doesn't care about graphics API
├─────────────────────────────────┤
│  RHI (abstract interface)        │  ← pure virtual, zero platform deps
│    setClearColor / clear /       │
│    setViewport / present         │
├─────────────────────────────────┤
│  RHIOpenGL (cross-platform)      │  ← gl* calls, shared across platforms
├─────────────────────────────────┤
│  Platform GL Context             │  ← macOS: GLView (NSOpenGLView)
│                                 │     Linux: GLX/EGL (future)
│                                 │     Windows: WGL (future)
└─────────────────────────────────┘
```

**Cross-platform strategy**: OpenGL API itself is portable (`glClear`, `glDrawArrays`).
Only context creation is platform-specific. Future Vulkan backend = just add `Render/Vulkan/`.

### RHI Interface

```cpp
struct RHIViewport { float x, y, width, height; };

class RHI {
public:
    virtual ~RHI() = default;
    virtual void setClearColor(float r, float g, float b, float a) = 0;
    virtual void clear() = 0;
    virtual void setViewport(const RHIViewport& vp) = 0;
    virtual void present() = 0;   // only platform-dependent call
};
```

### GLView (macOS Platform)

```objc
@interface GLView : NSOpenGLView
@property (readonly) NSSize drawableSize;   // retina-aware
- (void)makeGLContextCurrent;
@end
```

- OpenGL 4.1 Core Profile, double-buffered, depth 24, stencil 8
- `wantsBestResolutionOpenGLSurface` for Retina
- `drawableSize` = `convertSizeToBacking:bounds.size`

## Modules

### AMEEFramework (singleton)

```objc
@interface AMEEFramework : NSObject
+ (AMEEFramework*)getInstance;
- (AMEEAppController*)getAppController;
- (void)runApplicationMainWithArgc:(int)argc argv:(const char*[])argv;
@end
```

- `AMEE_USES_DYNAMIC_LIBS=1` — dylib loading mode
- `AppControllerClassName` — C string `"AMEEAppController"` (pre-NSString initialization)
- `kAMEEDidUnload` / `kAMEEDidQuit` — exported notification strings

### AMEEAppController (global singleton)

```objc
@interface AMEEAppController : NSObject <NSApplicationDelegate, AMEEGameLoopDelegate>
@property NSWindow *mainWindow;
@property GLView *glView;
@property AMEEGameLoop *gameLoop;
@end

extern AMEEAppController* GetAppController(void);
```

Owned by `_AMEEAppController` global. Created in `runApplicationMain` via `[[AMEEAppController alloc] init]`, then set as NSApp.delegate.

### AMEEGameLoop (CVDisplayLink driver)

```objc
@protocol AMEEGameLoopDelegate <NSObject>
@optional
- (void)gameLoopUpdate:(double)deltaTime time:(double)time;
- (void)gameLoopRender:(double)deltaTime time:(double)time;
@end

@interface AMEEGameLoop : NSObject
@property (weak) id<AMEEGameLoopDelegate> delegate;
@property (readonly) AMEETime *clock;
- (void)start;   // create + start CVDisplayLink
- (void)stop;
@end
```

Thread model: CVDisplayLink (high-prio thread) → `dispatch_async(main)` → update/render.

### AMEETime

```objc
@interface AMEETime : NSObject
@property (readonly) double deltaTime;          // scaled by timeScale
@property (readonly) double time;               // scaled accumulated
@property (readonly) double unscaledDeltaTime;  // raw
@property (readonly) double unscaledTime;       // raw accumulated
@property double timeScale;                     // 0=pause, 1=normal, 2=fast
@property double fixedDeltaTime;                // default 1/60
@property double maxDeltaTime;                  // default 0.25 (4fps clamp)
@property (readonly) NSUInteger frameCount;
- (void)updateWithTimestamp:(double)timestamp;  // from CVTimeStamp
@end
```

Source: `videoTime / videoTimeScale`. First frame = 0. Clamped to maxDeltaTime.

## Per-Frame Flow

```
VSync → displayLinkCallback() [CVDisplayLink thread]
        dispatch_async(main)
          handleDisplayLinkOutput()
            clock.update(timestamp)
            delegate.gameLoopUpdate(dt, t)
            delegate.gameLoopRender(dt, t)
              makeGLContextCurrent()
              setViewport(drawableSize)
              setClearColor(time-based color)
              clear()
              present()   → flushBuffer
```

## File Map

```
AMEEFramework/
  AMEEFramework.h          public header (umbrella)
  AMEEFramework.mm          singleton + bootstrap
  Core/
    AMEETime.h/mm           time management
    AMEEGameLoop.h/mm       CVDisplayLink game loop
  Render/
    RHITypes.h              shared types (RHIViewport)
    RHI.h                   abstract render interface
    GL/
      RHIOpenGL.h/mm        OpenGL 4.1 backend
  Platform/
    macOS/
      GLView.h/mm           NSOpenGLView + GL context
Sources/
  AMEEAppController.h       public header
  AMEEAppController.mm      window/menubar/lifecycle/render loop
Demo/
  main.mm                   app entry, framework loader
```

`AMEEFramework/` and `Sources/` → AMEEFramework target. `Demo/` → Demo target.

## Bugs Fixed

| # | Bug | Root Cause | Fix |
|---|-----|-----------|-----|
| 1 | Storyboard class mismatch | `customClass="AppDelegate"` vs actual `AMEEAppController` | Rename in Main.storyboard |
| 2 | Missing function implementations | `AMEEFunctionsForDynamicLibs` / `AMEESetExecuteMachHeader` declared only | Add stub implementations |
| 3 | Missing constant definitions | `kAMEEDidUnload` / `kAMEEDidQuit` extern only | Add string definitions |
| 4 | `NSApplicationMain` wrong usage | Commented 4-param iOS style in macOS code | Clean up to `NSApp` manual setup |
| 5 | `ViewController.m` excluded from build | `membershipExceptions` in pbxproj | Remove exception |
| 6 | Framework header `""` include | Module verifier rejects in public headers | Change to `<>` |
| 7 | Framework load returns nil | Path missing `/Contents/` | `bundlePath + "/Contents/Frameworks/..."` |
| 8 | Framework load returns nil (2) | `bundle.principalClass` unreliable without NSPrincipalClass | Use `NSClassFromString` |
| 9 | AppController nil as delegate | `getAppController` called before AppController created | `alloc init` before `setDelegate:` |
| 10 | OpenGL symbols undefined | BuildFile entry missing, not linked | Add via `OTHER_LDFLAGS` |
| 11 | Method name mismatch | `runApplicationMainWithArgs:` vs `WithArgc:` | Align declaration and call site |
| 12 | `id` type causes warning | Compiler can't resolve method on `id` | Use `AMEEFramework*` |

## Lessons Learned

1. **macOS app bundle path**: `[mainBundle bundlePath]` returns `App.app`, framework is at `App.app/Contents/Frameworks/`
2. **`bundle.principalClass`** depends on Info.plist `NSPrincipalClass`; prefer `NSClassFromString` for reliability
3. **AppController must be instantiated before setDelegate** — `getAppController()` is a getter, not a factory
4. **OpenGL.framework in Xcode 16 file-synced projects**: manual `PBXBuildFile` may not link; use `OTHER_LDFLAGS = ("-framework", OpenGL)`
5. **NSOpenGLView deprecated** in macOS 10.14+ (Apple wants Metal), but OpenGL 4.1 is still functional and cross-platform

## Roadmap

- [x] Game loop (CVDisplayLink, time management)
- [x] OpenGL 4.1 RHI (clear screen, cross-platform layer)
- [ ] Draw triangle (shader, vertex buffer, full pipeline)
- [ ] Input system (NSEvent → unified API)
- [ ] Asset management (texture, refcount, async load)
- [ ] Scene/entity/component tree
- [ ] Networking (SIGPIPE already handled)
- [ ] Vulkan backend (RHI interface ready)
- [ ] Scripting runtime (Lua/C# via dylib loading)
- [ ] Linux/Windows port (only Platform layer needs new files)
- [ ] iOS port (thin shell swap, same core)
