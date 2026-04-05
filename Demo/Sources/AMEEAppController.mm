#import "AMEEAppController.h"
#import "AMEEGameLoop.h"

AMEEAppController* _AMEEAppController = nil;

AMEEAppController* GetAppController()
{
    return _AMEEAppController;
}

@implementation AMEEAppController

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

    self.gameLoop = [[AMEEGameLoop alloc] initWithDelegate:self];
    [self.gameLoop start];

    NSLog(@"[AMEEAppController] Application launched");
}

- (void)applicationWillTerminate:(NSNotification *)aNotification
{
    [self.gameLoop stop];
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

    // App menu
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

    // Window menu
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
    NSUInteger styleMask = NSWindowStyleMaskTitled
        | NSWindowStyleMaskClosable
        | NSWindowStyleMaskMiniaturizable
        | NSWindowStyleMaskResizable;

    NSRect contentRect = NSMakeRect(0, 0, 1024, 768);
    self.mainWindow = [[NSWindow alloc] initWithContentRect:contentRect
                                                 styleMask:styleMask
                                                   backing:NSBackingStoreBuffered
                                                     defer:NO];
    self.mainWindow.title = @"AMEE Engine";
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
    // TODO: rendering
}

@end
