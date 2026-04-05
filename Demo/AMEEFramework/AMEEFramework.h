//
//  AMEEFramework.h
//  AMEEFramework
//
//  Created by Allen Enos on 2025/11/23.
//
#define AMEE_USES_DYNAMIC_LIBS 1

#import <Foundation/Foundation.h>

#import <AMEEFramework/AMEEAppController.h>

#include <mach-o/ldsyms.h>
typedef struct
#ifdef __LP64__
    mach_header_64
#else
    mach_header
#endif
    MachHeader;

//! Project version number for AMEEFramework.
FOUNDATION_EXPORT double AMEEFrameworkVersionNumber;

//! Project version string for AMEEFramework.
FOUNDATION_EXPORT const unsigned char AMEEFrameworkVersionString[];

// In this header, you should import all the public headers of your framework using statements like #import <AMEEFramework/PublicHeader.h>

#pragma once

// important app life-cycle events
__attribute__ ((visibility("default")))
@protocol AMEEFrameworkListener<NSObject>
@optional
- (void)AMEEDidUnload:(NSNotification*)notification;
- (void)AMEEDidQuit:(NSNotification*)notification;
@end


__attribute__ ((visibility("default")))
@interface AMEEFramework : NSObject
{
}


+ (AMEEFramework*)getInstance;

- (AMEEAppController*)getAppController;

//
- (void)setExecuteHeader:(const MachHeader*)header;

- (void)runApplicationMainWithArgc:(int)argc argv:(const char*[])argv;

@end
