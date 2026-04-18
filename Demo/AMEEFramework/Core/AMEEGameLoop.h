#import <Foundation/Foundation.h>

@class AMEETime;

@protocol AMEEGameLoopDelegate <NSObject>
@optional
- (void)gameLoopUpdate:(double)deltaTime time:(double)time;
- (void)gameLoopFixedUpdate:(double)fixedDeltaTime;
- (void)gameLoopRender:(double)deltaTime time:(double)time;
@end

@interface AMEEGameLoop : NSObject

@property (nonatomic, weak) id<AMEEGameLoopDelegate> delegate;
@property (nonatomic, readonly) BOOL isRunning;
@property (nonatomic, strong, readonly) AMEETime *clock;

- (instancetype)initWithDelegate:(id<AMEEGameLoopDelegate>)delegate;
- (void)start;
- (void)stop;

@end
