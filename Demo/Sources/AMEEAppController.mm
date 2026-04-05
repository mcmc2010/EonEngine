#import "AMEEAppController.h"
#import "AMEEGameLoop.h"
#include <memory>
#include "../AMEEFramework/Render/RHI.h"
#include "../AMEEFramework/Render/GL/RHIOpenGL.h"

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wdeprecated-declarations"
// NSOpenGLView is deprecated in macOS 10.14+ in favor of MTKView,
// but we intentionally use OpenGL 4.1 for cross-platform compatibility.
#import "../AMEEFramework/Platform/macOS/GLView.h"
#pragma clang diagnostic pop

// Declare conformance to AMEEGameLoopDelegate in implementation file
// to avoid exposing internal protocol in public header.
@interface AMEEAppController () <AMEEGameLoopDelegate>
@end

AMEEAppController* _AMEEAppController = nil;

AMEEAppController* GetAppController()
{
    return _AMEEAppController;
}

@implementation AMEEAppController {
    std::unique_ptr<RHI> _rhi;
}

- (id)init
{
    if ((self = _AMEEAppController = [super init]))
    {
    }
    return self;
}

#pragma mark - NSApplicationDelegate

- (void)applicationDidFinishLaunching:(NSNotification *)aNotification
{
    [self setupMenuBar];
    [self setupMainWindow];

    _rhi = std::make_unique<RHIOpenGL>();

    self.gameLoop = [[AMEEGameLoop alloc] initWithDelegate:self];
    [self.gameLoop start];

    NSLog(@"[AMEEAppController] Application launched with OpenGL 4.1");
}

- (void)applicationWillTerminate:(NSNotification *)aNotification
{
    [self.gameLoop stop];
    _rhi.reset();
    NSLog(@"[AMEEAppController] Application terminating");
}

- (BOOL)applicationSupportsSecureRestorableState:(NSApplication *)app
{
    return YES;
}

- (BOOL)applicationShouldTerminateAfterLastWindowClosed:(NSApplication *)sender
{
    return YES;
}

#pragma mark - Setup

- (void)setupMenuBar
{
    NSMenu *mainMenu = [[NSMenu alloc] init];
    [NSApp setMainMenu:mainMenu];

    NSMenuItem *appMenuItem = [[NSMenuItem alloc] init];
    [mainMenu addItem:appMenuItem];
    NSMenu *appMenu = [[NSMenu alloc] initWithTitle:@"AMEE Engine"];
    [appMenuItem setSubmenu:appMenu];

    [appMenu addItemWithTitle:@"About AMEE Engine"
                       action:@selector(orderFrontStandardAboutPanel:)
                keyEquivalent:@""];
    [appMenu addItem:[NSMenuItem separatorItem]];
    [appMenu addItemWithTitle:@"Quit AMEE Engine"
                       action:@selector(terminate:)
                keyEquivalent:@"q"];

    NSMenuItem *windowMenuItem = [[NSMenuItem alloc] init];
    [mainMenu addItem:windowMenuItem];
    NSMenu *windowMenu = [[NSMenu alloc] initWithTitle:@"Window"];
    [windowMenuItem setSubmenu:windowMenu];
    [NSApp setWindowsMenu:windowMenu];

    [windowMenu addItemWithTitle:@"Minimize"
                          action:@selector(performMiniaturize:)
                   keyEquivalent:@"m"];
    [windowMenu addItemWithTitle:@"Zoom"
                          action:@selector(performZoom:)
                   keyEquivalent:@""];
}

- (void)setupMainWindow
{
    NSRect contentRect = NSMakeRect(0, 0, 1024, 768);
    self.glView = [[GLView alloc] initWithFrame:contentRect];

    NSUInteger styleMask = NSWindowStyleMaskTitled
        | NSWindowStyleMaskClosable
        | NSWindowStyleMaskMiniaturizable
        | NSWindowStyleMaskResizable;

    self.mainWindow = [[NSWindow alloc] initWithContentRect:contentRect
                                                 styleMask:styleMask
                                                   backing:NSBackingStoreBuffered
                                                     defer:NO];
    [self.mainWindow setContentView:self.glView];
    self.mainWindow.title = @"AMEE Engine (OpenGL 4.1)";
    self.mainWindow.minSize = NSMakeSize(320, 240);
    [self.mainWindow center];
    [self.mainWindow makeKeyAndOrderFront:nil];
}

#pragma mark - AMEEGameLoopDelegate

- (void)gameLoopUpdate:(double)deltaTime time:(double)time
{
    // TODO: game logic update
}

- (void)gameLoopRender:(double)deltaTime time:(double)time
{
    [self.glView makeGLContextCurrent];

    NSSize size = [self.glView drawableSize];
    _rhi->setViewport({0.0f, 0.0f, (float)size.width, (float)size.height});

    float t = (float)time * 0.5f;
    _rhi->setClearColor(0.15f + 0.1f * sinf(t), 0.15f, 0.2f + 0.05f * sinf(t * 0.7f), 1.0f);

    _rhi->clear();
    _rhi->present();
}

@end
