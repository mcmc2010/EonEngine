#pragma once
#import <CoreVideo/CoreVideo.h>
#include "../IPlatformLoop.h"

class MacGameLoop : public IPlatformLoop {
public:
    MacGameLoop();
    ~MacGameLoop() override;

    bool start(TickCallback callback) override;
    void stop() override;
    bool isRunning() const override;

    double getDeltaTime() const override;
    double getTime() const override;
    unsigned int getFrameCount() const override;

    void handleDisplayLinkOutput(const CVTimeStamp* outputTime);

private:
    CVDisplayLinkRef _displayLink;
    TickCallback _callback;

    double _lastTimestamp;
    double _deltaTime;
    double _time;
    unsigned int _frameCount;
    double _fixedDeltaTime;
    double _maxDeltaTime;
    double _accumulatedFixedTime;
    double _fixedTime;
};
