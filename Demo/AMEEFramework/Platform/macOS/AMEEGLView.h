#import <Cocoa/Cocoa.h>

@interface GLView : NSOpenGLView

@property (nonatomic, readonly) NSSize drawableSize;

- (void)makeGLContextCurrent;
+ (NSOpenGLPixelFormat *)defaultPixelFormat;

@end
