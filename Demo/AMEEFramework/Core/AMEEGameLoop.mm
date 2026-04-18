#import "AMEEGameLoop.h"
#import "AMEETime.h"
#import <CoreVideo/CoreVideo.h>

static CVReturn displayLinkCallback(CVDisplayLinkRef displayLink,
                                    const CVTimeStamp *inNow,
                                    const CVTimeStamp *inOutputTime,
                                    CVOptionFlags flagsIn,
                                    CVOptionFlags *flagsOut,
                                    void *displayLinkContext);

@interface AMEEGameLoop ()
@property (nonatomic, assign) CVDisplayLinkRef displayLink;
@property (nonatomic, strong) AMEETime *clock;
@end

@implementation AMEEGameLoop

- (instancetype)initWithDelegate:(id<AMEEGameLoopDelegate>)delegate
{
    if ((self = [super init]))
    {
        _delegate = delegate;
        _clock = [[AMEETime alloc] init];
    }
    return self;
}

- (BOOL)isRunning
{
    return _displayLink != NULL;
}

- (void)start
{
    if (self.isRunning) return;

    CVDisplayLinkRef displayLink = NULL;
    CVReturn status = CVDisplayLinkCreateWithCGDisplay(CGMainDisplayID(), &displayLink);
    if (status != kCVReturnSuccess)
    {
        NSLog(@"[AMEEGameLoop] Failed to create CVDisplayLink: %d", status);
        return;
    }

    CVDisplayLinkSetOutputCallback(displayLink, displayLinkCallback, (__bridge void *)self);

    status = CVDisplayLinkStart(displayLink);
    if (status != kCVReturnSuccess)
    {
        NSLog(@"[AMEEGameLoop] Failed to start CVDisplayLink: %d", status);
        CVDisplayLinkRelease(displayLink);
        return;
    }

    self.displayLink = displayLink;
    NSLog(@"[AMEEGameLoop] Started (display refresh: %.0f Hz)",
          CVDisplayLinkGetActualOutputVideoRefreshPeriod(displayLink) == 0 ? 60 :
          1.0 / CVDisplayLinkGetActualOutputVideoRefreshPeriod(displayLink));
}

- (void)stop
{
    if (!self.isRunning) return;

    CVDisplayLinkStop(self.displayLink);
    CVDisplayLinkRelease(self.displayLink);
    self.displayLink = NULL;
    NSLog(@"[AMEEGameLoop] Stopped");
}

- (void)dealloc
{
    [self stop];
}

#pragma mark - Internal

- (void)handleDisplayLinkOutput:(const CVTimeStamp *)outputTime
{
    double timestamp = (double)outputTime->videoTime / (double)outputTime->videoTimeScale;
    [self.clock updateWithTimestamp:timestamp];

    while ([self.clock fixedTime] > 0) {
        if ([self.delegate respondsToSelector:@selector(gameLoopFixedUpdate:)])
            [self.delegate gameLoopFixedUpdate:self.clock.fixedDeltaTime];
    }

    if ([self.delegate respondsToSelector:@selector(gameLoopUpdate:time:)])
        [self.delegate gameLoopUpdate:self.clock.deltaTime time:self.clock.time];

    if ([self.delegate respondsToSelector:@selector(gameLoopRender:time:)])
        [self.delegate gameLoopRender:self.clock.deltaTime time:self.clock.time];
}

static CVReturn displayLinkCallback(CVDisplayLinkRef displayLink,
                                    const CVTimeStamp *inNow,
                                    const CVTimeStamp *inOutputTime,
                                    CVOptionFlags flagsIn,
                                    CVOptionFlags *flagsOut,
                                    void *displayLinkContext)
{
    @autoreleasepool
    {
        AMEEGameLoop *loop = (__bridge AMEEGameLoop *)displayLinkContext;
        dispatch_async(dispatch_get_main_queue(), ^{
            [loop handleDisplayLinkOutput:inOutputTime];
        });
    }
    return kCVReturnSuccess;
}

@end
