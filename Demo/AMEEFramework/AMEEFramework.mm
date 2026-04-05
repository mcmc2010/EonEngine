//
//  AMEEFramework.mm
//  Demo
//
//  Created by Allen Enos on 2025/11/23.
//

#include <csignal>

#include "AMEEFramework.h"

// WARNING: this MUST be c decl (NSString ctor will be called after +load, so we cant really change its value)
const char* AppControllerClassName = "AMEEAppController";
#if AMEE_USES_DYNAMIC_LIBS
extern "C" void AMEEFunctionsForDynamicLibs();
#endif

extern "C" void AMEESetExecuteMachHeader(const MachHeader* header);

extern "C" __attribute__((visibility("default"))) NSString* const kAMEEDidUnload = @"kAMEEDidUnload";
extern "C" __attribute__((visibility("default"))) NSString* const kAMEEDidQuit = @"kAMEEDidQuit";

#if AMEE_USES_DYNAMIC_LIBS
static const MachHeader* g_ExecuteMachHeader = nullptr;

void AMEEFunctionsForDynamicLibs()
{
    // TODO: register dynamic library functions
}

void AMEESetExecuteMachHeader(const MachHeader* header)
{
    g_ExecuteMachHeader = header;
}
#endif

@implementation AMEEFramework
{
    int runCount;
}


AMEEFramework* g_pAMEEFramework = nil;

+ (AMEEFramework*)getInstance
{
    if (g_pAMEEFramework == nil)
    {
        g_pAMEEFramework = [[AMEEFramework alloc] init];
    }
    return g_pAMEEFramework;
}

- (AMEEAppController*)getAppController
{
    return GetAppController();
}

- (void)setExecuteHeader:(const MachHeader*)header
{
    AMEESetExecuteMachHeader(header);
}

- (void)frameworkWarmup:(int)argc argv:(const char*[])argv
{
#if AMEE_USES_DYNAMIC_LIBS
    AMEEFunctionsForDynamicLibs();
#endif
    
    
    //UnityInitTrampoline();
    //UnityInitRuntime(argc, argv);
    
    //RegisterFeatures();
    
    // iOS terminates open sockets when an application enters background mode.
    // The next write to any of such socket causes SIGPIPE signal being raised,
    // even if the request has been done from scripting side. This disables the
    // signal and allows Mono to throw a proper C# exception.
    std::signal(SIGPIPE, SIG_IGN);
}

- (void)runApplicationMainWithArgc:(int)argc argv:(const char*[])argv
{
    self->runCount += 1;
    [self frameworkWarmup: argc argv: argv];

    // Set up application programmatically (no Storyboard dependency)
    NSApplication *app = [NSApplication sharedApplication];
    app.activationPolicy = NSApplicationActivationPolicyRegular;
    [app setDelegate:[self getAppController]];
    [app activateIgnoringOtherApps:YES];

    [app run];
}


@end
