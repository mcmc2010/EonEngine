#import "AMEETime.h"

@interface AMEETime ()
@property (nonatomic, assign) double lastTimestamp;
@end

@implementation AMEETime

- (instancetype)init
{
    if ((self = [super init]))
    {
        _timeScale = 1.0;
        _fixedDeltaTime = 1.0 / 60.0;
        _maxDeltaTime = 0.25;
        _frameCount = 0;
    }
    return self;
}

- (void)updateWithTimestamp:(double)timestamp
{
    double rawDelta = timestamp - self.lastTimestamp;
    self.lastTimestamp = timestamp;

    // First frame has no valid delta
    if (self.frameCount == 0)
    {
        rawDelta = 0;
    }

    // Clamp to max delta time to prevent spiral of death
    if (rawDelta > self.maxDeltaTime)
        rawDelta = self.maxDeltaTime;

    _unscaledDeltaTime = rawDelta;
    _unscaledTime += rawDelta;

    double scaledDelta = rawDelta * self.timeScale;
    _deltaTime = scaledDelta;
    _time += scaledDelta;

    _frameCount++;
}

@end
