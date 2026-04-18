#import "MacGameLoop.h"
#import "../../Core/Log/AMEELog.h"
#include "../../Core/Platform/IPlatformLoop.h"

static CVReturn displayLinkCallback(CVDisplayLinkRef displayLink,
                                    const CVTimeStamp *inNow,
                                    const CVTimeStamp *inOutputTime,
                                    CVOptionFlags flagsIn,
                                    CVOptionFlags *flagsOut,
                                    void *displayLinkContext);

MacGameLoop::MacGameLoop()
    : _displayLink(nullptr)
    , _lastTimestamp(0)
    , _deltaTime(0)
    , _time(0)
    , _frameCount(0)
    , _fixedDeltaTime(1.0 / 60.0)
    , _maxDeltaTime(0.25)
    , _accumulatedFixedTime(0)
    , _fixedTime(0)
{
}

MacGameLoop::~MacGameLoop()
{
    stop();
}

bool MacGameLoop::start(TickCallback callback)
{
    if (isRunning()) return false;

    _callback = std::move(callback);

    CVDisplayLinkRef displayLink = nullptr;
    CVReturn status = CVDisplayLinkCreateWithCGDisplay(CGMainDisplayID(), &displayLink);
    if (status != kCVReturnSuccess) {
        AMEE_LOG_ERROR("MacGameLoop", "Failed to create CVDisplayLink: %d", status);
        return false;
    }

    CVDisplayLinkSetOutputCallback(displayLink, displayLinkCallback, this);

    status = CVDisplayLinkStart(displayLink);
    if (status != kCVReturnSuccess) {
        AMEE_LOG_ERROR("MacGameLoop", "Failed to start CVDisplayLink: %d", status);
        CVDisplayLinkRelease(displayLink);
        return false;
    }

    _displayLink = displayLink;

    double refreshHz = CVDisplayLinkGetActualOutputVideoRefreshPeriod(displayLink);
    AMEE_LOG_INFO("MacGameLoop", "Started (%.0f Hz)", refreshHz > 0 ? 1.0 / refreshHz : 60.0);

    return true;
}

void MacGameLoop::stop()
{
    if (!isRunning()) return;

    CVDisplayLinkStop(_displayLink);
    CVDisplayLinkRelease(_displayLink);
    _displayLink = nullptr;

    AMEE_LOG_INFO("MacGameLoop", "Stopped");
}

bool MacGameLoop::isRunning() const
{
    return _displayLink != nullptr;
}

double MacGameLoop::getDeltaTime() const
{
    return _deltaTime;
}

double MacGameLoop::getTime() const
{
    return _time;
}

unsigned int MacGameLoop::getFrameCount() const
{
    return _frameCount;
}

void MacGameLoop::handleDisplayLinkOutput(const CVTimeStamp* outputTime)
{
    double timestamp = (double)outputTime->videoTime / (double)outputTime->videoTimeScale;

    double rawDelta = timestamp - _lastTimestamp;
    _lastTimestamp = timestamp;

    if (_frameCount == 0) {
        rawDelta = 0;
    }

    if (rawDelta > _maxDeltaTime) {
        rawDelta = _maxDeltaTime;
    }

    _deltaTime = rawDelta;
    _time += rawDelta;
    _frameCount++;

    _accumulatedFixedTime += rawDelta;
    while (_accumulatedFixedTime >= _fixedDeltaTime) {
        _fixedTime += _fixedDeltaTime;
        _accumulatedFixedTime -= _fixedDeltaTime;
    }

    if (_callback) {
        _callback(rawDelta, _time);
    }
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
        MacGameLoop* loop = static_cast<MacGameLoop*>(displayLinkContext);
        dispatch_async(dispatch_get_main_queue(), ^{
            loop->handleDisplayLinkOutput(inOutputTime);
        });
    }
    return kCVReturnSuccess;
}
