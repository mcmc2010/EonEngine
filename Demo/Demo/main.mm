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
    NSString* bundlePath = nil;
    bundlePath = [[NSBundle mainBundle] bundlePath];
    bundlePath = [bundlePath stringByAppendingString: @"/Frameworks/AMEEFramework.framework"];

    NSBundle* bundle = [NSBundle bundleWithPath: bundlePath];
    if ([bundle isLoaded] == false) [bundle load];

    AMEEFramework* fw = [bundle.principalClass getInstance];
    if (![fw getAppController])
    {
        // unity is not initialized
        [fw setExecuteHeader: &_mh_execute_header];
    }
    return fw;
}

//
int main(int argc, const char * argv[]) {
    @autoreleasepool {
        // Setup code that might create autoreleased objects goes here.
        id fw = AMEEFrameworkLoad();
        if (fw == nullptr) {
            throw "AMEEFramework Exception";
            return -1;
        }
        [fw runApplicationMainWithArgc:argc argv: argv];
        return 0;
    }
    //return NSApplicationMain(argc, argv);
}
