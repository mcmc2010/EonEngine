#pragma once
#include <functional>

class IPlatformLoop {
public:
    using TickCallback = std::function<void(double dt, double time)>;

    virtual ~IPlatformLoop() = default;

    virtual bool start(TickCallback callback) = 0;
    virtual void stop() = 0;
    virtual bool isRunning() const = 0;

    virtual double getDeltaTime() const = 0;
    virtual double getTime() const = 0;
    virtual unsigned int getFrameCount() const = 0;
};
