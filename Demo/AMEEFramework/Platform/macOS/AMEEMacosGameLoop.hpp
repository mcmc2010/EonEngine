#ifndef __AMEE_MACOSGAMELOOP_H__
#define __AMEE_MACOSGAMELOOP_H__
#pragma once
#import <CoreVideo/CoreVideo.h>
#include "../../Core/Platform/IAMEEPlatformLoop.hpp"

namespace AMEE {

class MacosGameLoop : public IPlatformLoop {
public:
    MacosGameLoop();
    ~MacosGameLoop() override;

    bool start(TickCallback callback) override;
    void stop() override;
    bool isRunning() const override;

    double getDeltaTime() const override;
    double getTime() const override;
    unsigned int getFrameCount() const override;

    void handleDisplayLinkOutput(const CVTimeStamp* outputTime);

private:
    CVDisplayLinkRef m_DisplayLink;
    TickCallback m_Callback;

    double m_LastTimestamp;
    double m_DeltaTime;
    double m_Time;
    unsigned int m_FrameCount;
    double m_FixedDeltaTime;
    double m_MaxDeltaTime;
    double m_AccumulatedFixedTime;
    double m_FixedTime;
};

} // namespace AMEE

#endif // __AMEE_MACOSGAMELOOP_H__
