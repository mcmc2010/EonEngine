#ifndef __AMEE_MACOSAPPLICATION_H__
#define __AMEE_MACOSAPPLICATION_H__
#pragma once
#include "../../Core/AMEEApplication.hpp"

namespace AMEE {

class MacosApplication : public Application {
protected:
    std::unique_ptr<IPlatformWindow> CreatePlatformWindow() override;
    std::unique_ptr<IPlatformGLContext> CreatePlatformGLContext() override;
    std::unique_ptr<IPlatformLoop> CreatePlatformGameLoop() override;
    std::unique_ptr<RHI> CreateRHI() override;
};

} // namespace AMEE

#endif // __AMEE_MACOSAPPLICATION_H__
