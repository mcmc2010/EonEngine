#import "AMEEMacosGameLoop.hpp"
#import "../../Core/Log/AMEELog.hpp"
#include "../../Core/Platform/IAMEEPlatformLoop.hpp"
#import <AppKit/AppKit.h>
#import <CoreFoundation/CoreFoundation.h>

static id g_DisplayChangeObserver = nil;

static CVReturn displayLinkCallback(CVDisplayLinkRef displayLink,
                                    const CVTimeStamp *inNow,
                                    const CVTimeStamp *inOutputTime,
                                    CVOptionFlags flagsIn,
                                    CVOptionFlags *flagsOut,
                                    void *displayLinkContext);

namespace AMEE {

MacosGameLoop::MacosGameLoop()
    : m_DisplayLink(nullptr)
    , m_LastTimestamp(0)
    , m_DeltaTime(0)
    , m_Time(0)
    , m_FrameCount(0)
    , m_FixedDeltaTime(1.0 / 60.0)
    , m_MaxDeltaTime(0.25)
    , m_AccumulatedFixedTime(0)
    , m_FixedTime(0)
{
}

MacosGameLoop::~MacosGameLoop()
{
    stop();
}

bool MacosGameLoop::start(RenderCallback renderCb, FixedCallback fixedCb)
{
    if (isRunning()) return false;

    m_RenderCallback = std::move(renderCb);
    m_FixedCallback  = std::move(fixedCb);

    // Watch for display changes (sleep, resolution switch, etc.)
    if (!g_DisplayChangeObserver) {
        g_DisplayChangeObserver = [[NSNotificationCenter defaultCenter]
            addObserverForName:NSApplicationDidChangeScreenParametersNotification
            object:nil
            queue:[NSOperationQueue mainQueue]
            usingBlock:^(NSNotification* note) {
                AMEE_LOG_WARN("MacosGameLoop", "Display configuration changed");
            }];
    }

    CVDisplayLinkRef displayLink = nullptr;
    CVReturn status = CVDisplayLinkCreateWithCGDisplay(CGMainDisplayID(), &displayLink);
    if (status != kCVReturnSuccess) {
        AMEE_LOG_ERROR("MacosGameLoop", "Failed to create CVDisplayLink: %d", status);
        return false;
    }

    CVDisplayLinkSetOutputCallback(displayLink, displayLinkCallback, this);

    status = CVDisplayLinkStart(displayLink);
    if (status != kCVReturnSuccess) {
        AMEE_LOG_ERROR("MacosGameLoop", "Failed to start CVDisplayLink: %d", status);
        CVDisplayLinkRelease(displayLink);
        return false;
    }

    m_DisplayLink = displayLink;

    double refreshHz = CVDisplayLinkGetActualOutputVideoRefreshPeriod(displayLink);
    AMEE_LOG_INFO("MacosGameLoop", "Started (%.0f Hz)", refreshHz > 0 ? 1.0 / refreshHz : 60.0);

    return true;
}

void MacosGameLoop::stop()
{
    if (!isRunning()) return;

    CVDisplayLinkStop(m_DisplayLink);
    CVDisplayLinkRelease(m_DisplayLink);
    m_DisplayLink = nullptr;

    AMEE_LOG_INFO("MacosGameLoop", "Stopped");
}

bool MacosGameLoop::isRunning() const
{
    return m_DisplayLink != nullptr;
}

double MacosGameLoop::getDeltaTime() const
{
    return m_DeltaTime;
}

double MacosGameLoop::getTime() const
{
    return m_Time;
}

unsigned int MacosGameLoop::getFrameCount() const
{
    return m_FrameCount;
}

void MacosGameLoop::handleDisplayLinkOutput(const CVTimeStamp* outputTime)
{
    double timestamp = CFAbsoluteTimeGetCurrent();

    double rawDelta = timestamp - m_LastTimestamp;
    m_LastTimestamp = timestamp;

    if (m_FrameCount == 0) {
        rawDelta = 0;
    }

    if (rawDelta > m_MaxDeltaTime) {
        rawDelta = m_MaxDeltaTime;
    }

    m_DeltaTime = rawDelta;
    m_Time += rawDelta;
    m_FrameCount++;

    m_AccumulatedFixedTime += rawDelta;
    while (m_AccumulatedFixedTime >= m_FixedDeltaTime) {
        m_FixedTime += m_FixedDeltaTime;
        m_AccumulatedFixedTime -= m_FixedDeltaTime;
        if (m_FixedCallback) {
            m_FixedCallback(m_FixedDeltaTime);
        }
    }

    if (m_RenderCallback) {
        double Alpha = (m_FixedDeltaTime > 0) ? (m_AccumulatedFixedTime / m_FixedDeltaTime) : 0.0;
        m_RenderCallback(rawDelta, m_Time, Alpha);
    }

    // Heartbeat: log every 60 frames so we can see when it stops
    if (m_FrameCount % 600 == 0) {
        AMEE_LOG_INFO("MacosGameLoop", "Heartbeat: frame %u, time %.1fs",
                      m_FrameCount, m_Time);
    }
}

} // namespace AMEE

static CVReturn displayLinkCallback(CVDisplayLinkRef displayLink,
                                    const CVTimeStamp *inNow,
                                    const CVTimeStamp *inOutputTime,
                                    CVOptionFlags flagsIn,
                                    CVOptionFlags *flagsOut,
                                    void *displayLinkContext)
{
    @autoreleasepool
    {
        AMEE::MacosGameLoop* loop = static_cast<AMEE::MacosGameLoop*>(displayLinkContext);
        dispatch_async(dispatch_get_main_queue(), ^{
            @autoreleasepool {
                loop->handleDisplayLinkOutput(inOutputTime);
            }
        });
    }
    return kCVReturnSuccess;
}
