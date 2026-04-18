#import <Foundation/Foundation.h>

@interface AMEETime : NSObject

// Scaled time (affected by timeScale)
@property (nonatomic, readonly) double deltaTime;
@property (nonatomic, readonly) double time;

// Unscaled time (real elapsed, not affected by timeScale)
@property (nonatomic, readonly) double unscaledDeltaTime;
@property (nonatomic, readonly) double unscaledTime;

// Fixed timestep for physics/logic updates
@property (nonatomic, assign) double fixedDeltaTime;
@property (nonatomic, assign) double maxDeltaTime;

// Time scale (1.0 = normal, 0.0 = paused, 2.0 = double speed)
@property (nonatomic, assign) double timeScale;

// Frame counter
@property (nonatomic, readonly) NSUInteger frameCount;

// Fixed update accumulator (for physics)
@property (nonatomic, readonly) double fixedTime;
@property (nonatomic, readonly) double accumulatedFixedTime;

- (void)updateWithTimestamp:(double)timestamp;

@end
