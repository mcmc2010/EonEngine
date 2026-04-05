#import <Cocoa/Cocoa.h>

@class AMEEGameLoop;
@class GLView;

__attribute__ ((visibility("default")))
@interface AMEEAppController : NSObject <NSApplicationDelegate>

@property (nonatomic, strong) NSWindow *mainWindow;
@property (nonatomic, strong) GLView *glView;
@property (nonatomic, strong) AMEEGameLoop *gameLoop;

@end

#ifdef __cplusplus
extern "C" {
#endif

extern AMEEAppController* _AMEEAppController;
extern AMEEAppController* GetAppController(void);

#ifdef __cplusplus
}
#endif
