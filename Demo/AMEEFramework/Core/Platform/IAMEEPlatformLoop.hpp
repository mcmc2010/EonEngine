#ifndef __AMEE_IPLATFORMLOOP_H__
#define __AMEE_IPLATFORMLOOP_H__
#pragma once
#include <functional>

namespace AMEE {


class IPlatformLoop {
public:
    using RenderCallback = std::function<void(double dt, double totalTime)>;
    using FixedCallback  = std::function<void(double fixedDt)>;

    virtual ~IPlatformLoop() = default;

    virtual bool start(RenderCallback renderCb, FixedCallback fixedCb = nullptr) = 0;
    virtual void stop() = 0;
    virtual bool isRunning() const = 0;

    virtual double getDeltaTime() const = 0;
    virtual double getTime() const = 0;
    virtual unsigned int getFrameCount() const = 0;
};
} // namespace AMEE

#endif // __AMEE_IPLATFORMLOOP_H__