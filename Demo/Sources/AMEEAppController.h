#import <Cocoa/Cocoa.h>

@protocol AMEEGameLoopDelegate;
@class AMEEGameLoop;

__attribute__ ((visibility("default")))
@interface AMEEAppController : NSObject <NSApplicationDelegate, AMEEGameLoopDelegate>

@property (nonatomic, strong) NSWindow *mainWindow;
@property (nonatomic, strong) AMEEGameLoop *gameLoop;

@end

// accessing app controller
#ifdef __cplusplus
extern "C" {
#endif

extern AMEEAppController* _AMEEAppController;
extern AMEEAppController* GetAppController(void);

#ifdef __cplusplus
} // extern "C"
#endif
