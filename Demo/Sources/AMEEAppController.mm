#import "AMEEAppController.h"
#include "DemoApp.hpp"

@interface AMEEAppController () <NSApplicationDelegate>
@end

static AMEE::DemoApp* g_pDemoApp = nullptr;

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

    g_pDemoApp = new AMEE::DemoApp();

    AMEE::ApplicationConfig config;
    config.Width = 1024;
    config.Height = 768;
    config.Title = "AMEE Engine (OpenGL 4.1)";
    config.MinWidth = 320;
    config.MinHeight = 240;

    if (!g_pDemoApp->Init(config)) {
        AMEE_LOG_ERROR("App", "Failed to initialize application");
        delete g_pDemoApp;
        g_pDemoApp = nullptr;
        return;
    }

    g_pDemoApp->Run();
}

- (void)applicationWillTerminate:(NSNotification *)aNotification
{
    if (g_pDemoApp) {
        g_pDemoApp->Shutdown();
        delete g_pDemoApp;
        g_pDemoApp = nullptr;
    }
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

@end
