#ifndef __AMEE_APPCONTROLLER_H__
#define __AMEE_APPCONTROLLER_H__
#import <Cocoa/Cocoa.h>

__attribute__ ((visibility("default")))
@interface AMEEAppController : NSObject <NSApplicationDelegate>
@end

#ifdef __cplusplus
extern "C" {
#endif

extern AMEEAppController* _AMEEAppController;
extern AMEEAppController* GetAppController(void);

#ifdef __cplusplus
}
#endif

#endif // __AMEE_APPCONTROLLER_H__
