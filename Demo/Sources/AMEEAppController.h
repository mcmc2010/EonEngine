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
