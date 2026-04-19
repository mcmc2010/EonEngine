#import "AMEEMacosApplication.hpp"
#import "AMEEMacosWindow.hpp"
#import "AMEEMacosGLContext.hpp"
#import "AMEEMacosGameLoop.hpp"
#import "../../Render/AMEERHI.hpp"
#import "../../Platform/macOS/GL/AMEERHIOpenGL.hpp"
#import "../../Core/Log/AMEELog.hpp"

namespace AMEE {

std::unique_ptr<IPlatformWindow> MacosApplication::CreatePlatformWindow()
{
    return std::make_unique<MacosWindow>();
}

std::unique_ptr<IPlatformGLContext> MacosApplication::CreatePlatformGLContext()
{
    return std::make_unique<MacosGLContext>();
}

std::unique_ptr<IPlatformLoop> MacosApplication::CreatePlatformGameLoop()
{
    return std::make_unique<MacosGameLoop>();
}

std::unique_ptr<RHI> MacosApplication::CreateRHI()
{
    return std::make_unique<RHIOpenGL>();
}

} // namespace AMEE
