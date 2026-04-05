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
  └── Frameworks/
        └── AMEEFramework.framework              (engine core)
              ├── AMEEFramework.mm                (singleton, warmup, app bootstrap)
              ├── Core/AMEETime.mm                (deltaTime, timeScale)
              ├── Core/AMEEGameLoop.mm            (CVDisplayLink → update → render)
              └── Sources/AMEEAppController.mm    (window, menubar, lifecycle)
```

**2 Targets**: `Demo` (app) depends on `AMEEFramework` (framework), embedded via `@rpath`.

## Startup Sequence

```
main()
  AMEEFrameworkLoad()
    NSBundle load AMEEFramework.framework
    AMEEFramework.getInstance
    setExecuteHeader(&_mh_execute_header)       // Mach-O for dynamic libs
  runApplicationMain()
    frameworkWarmup()
      AMEEFunctionsForDynamicLibs()             // TODO: register dylib funcs
      signal(SIGPIPE, SIG_IGN)                  // prevent socket crash
    [NSApplication sharedApplication]
    app.activationPolicy = Regular
    [app setDelegate: AMEEAppController]
    [app run]
      → applicationDidFinishLaunching:
          setupMenuBar()
          setupMainWindow()                       // 1024×768, code-created
          AMEEGameLoop.start                     // CVDisplayLink → main queue
```

No Storyboard. No NIB. Everything is code-created.

## Modules

### AMEEFramework (singleton)

```objc
// framework entry point
@interface AMEEFramework : NSObject
+ (AMEEFramework*)getInstance;
- (AMEEAppController*)getAppController;
- (void)runApplicationMainWithArgs:(int)argc argv:(const char*[])argv;
@end
```

- `AMEE_USES_DYNAMIC_LIBS=1` — dylib loading mode
- `AppControllerClassName` — C string `"AMEEAppController"` (pre-NSString initialization)
- `kAMEEDidUnload` / `kAMEEDidQuit` — exported notification strings

### AMEEAppController (global singleton)

```objc
// bridge between platform and engine
@interface AMEEAppController : NSObject <NSApplicationDelegate, AMEEGameLoopDelegate>
@property NSWindow *mainWindow;
@property AMEEGameLoop *gameLoop;
@end

// C access (exported from dynamic lib)
extern AMEEAppController* GetAppController(void);
```

Owned by `_AMEEAppController` global. Created by framework, set as NSApp.delegate before `[NSApp run]`.

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

Source: `videoTime / videoTimeScale`. First frame = 0. Clamped to maxDeltaTime to prevent spiral of death.

## Per-Frame Flow

```
VSync → displayLinkCallback() [CVDisplayLink thread]
        dispatch_async(main)
          handleDisplayLinkOutput()
            clock.update(timestamp)
            delegate.gameLoopUpdate(dt, t)
            delegate.gameLoopRender(dt, t)
```

## File Map

```
AMEEFramework/
  AMEEFramework.h          public header (umbrella)
  AMEEFramework.mm          singleton + bootstrap
  Core/
    AMEETime.h/mm           time management
    AMEEGameLoop.h/mm       CVDisplayLink game loop
Sources/
  AMEEAppController.h       public header
  AMEEAppController.mm      window/menubar/lifecycle
Demo/
  main.mm                   app entry, framework loader
```

`AMEEFramework/` and `Sources/` → AMEEFramework target. `Demo/` → Demo target.

## Roadmap

- [ ] Metal render pipeline (MTLDevice, MTKView, shaders)
- [ ] Input system (NSEvent → unified API)
- [ ] Asset management (MTLTexture, refcount, async load)
- [ ] Scene/entity/component tree
- [ ] Networking (SIGPIPE already handled)
- [ ] Scripting runtime (Lua/C# via dylib loading)
- [ ] iOS port (thin shell swap, same core)
