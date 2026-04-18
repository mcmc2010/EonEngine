//
//  main.m
//  Demo
//
//  Created by Allen Enos on 2025/11/23.
//

#import <Cocoa/Cocoa.h>
#include <AMEEFramework/AMEEFramework.h>

AMEEFramework* AMEEFrameworkLoad()
{
    //
    // /Users/silly/Library/Developer/Xcode/DerivedData/Demo-dodxwxvlvbjyfcahpbxjwxdvzoln/Build/Products/Debug/Demo.app/Contents/Frameworks
    NSString* bundlePath = [[NSBundle mainBundle] bundlePath];
    bundlePath = [bundlePath stringByAppendingString:@"/Contents/Frameworks/AMEEFramework.framework"];

    NSBundle* bundle = [NSBundle bundleWithPath:bundlePath];
    if (!bundle || ![bundle load]) {
        NSLog(@"[main] Failed to load AMEEFramework from: %@", bundlePath);
        return nil;
    }

    // Use NSClassFromString directly — more reliable than bundle.principalClass
    // which depends on NSPrincipalClass being correctly set in Info.plist
    Class principalClass = NSClassFromString(@"AMEEFramework");
    if (!principalClass) {
        principalClass = bundle.principalClass;
    }
    if (!principalClass) {
        NSLog(@"[main] AMEEFramework class not found after bundle load");
        return nil;
    }

    AMEEFramework* fw = [principalClass getInstance];
    if (![fw getAppController])
    {
        [fw setExecuteHeader: &_mh_execute_header];
    }
    return fw;
}

//
int main(int argc, const char * argv[]) {
    @autoreleasepool {
        // Setup code that might create autoreleased objects goes here.
        AMEEFramework *fw = AMEEFrameworkLoad();
        if (fw == nil) {
            throw "AMEEFramework Exception";
        }
        [fw runApplicationMainWithArgc:argc argv: argv];
        return 0;
    }
    //return NSApplicationMain(argc, argv);
}
