#import "AMEEMacosGameLoop.hpp"
#import "../../Core/Log/AMEELog.hpp"
#include "../../Core/Platform/IAMEEPlatformLoop.hpp"

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

bool MacosGameLoop::start(TickCallback callback)
{
    if (isRunning()) return false;

    m_Callback = std::move(callback);

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
    double timestamp = (double)outputTime->videoTime / (double)outputTime->videoTimeScale;

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
    }

    if (m_Callback) {
        m_Callback(rawDelta, m_Time);
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
            loop->handleDisplayLinkOutput(inOutputTime);
        });
    }
    return kCVReturnSuccess;
}
