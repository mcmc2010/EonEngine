#import "AMEEAppController.h"
#include <memory>
#include "../AMEEFramework/Render/RHI.h"
#include "../AMEEFramework/Render/GL/RHIOpenGL.h"
#include "../AMEEFramework/Render/Shader/GL/GLShader.h"
#include "../AMEEFramework/Core/Math/AMEEMath.h"
#include "../AMEEFramework/Core/Log/AMEELog.h"
#include "../AMEEFramework/Platform/macOS/MacWindow.h"
#include "../AMEEFramework/Platform/macOS/MacGLContext.h"
#include "../AMEEFramework/Platform/macOS/MacGameLoop.h"

@interface AMEEAppController () <NSApplicationDelegate>
@end

AMEEAppController* _AMEEAppController = nil;

AMEEAppController* GetAppController()
{
    return _AMEEAppController;
}

@implementation AMEEAppController {
    std::unique_ptr<IPlatformWindow> _window;
    std::unique_ptr<IPlatformGLContext> _glContext;
    std::unique_ptr<IPlatformLoop> _gameLoop;
    std::unique_ptr<RHI> _rhi;
    std::unique_ptr<GLShaderProgram> _triangleShader;
    uint32_t _triangleVAO;
    uint32_t _triangleVBO;
}

- (id)init
{
    if ((self = _AMEEAppController = [super init]))
    {
    }
    return self;
}

#pragma mark - NSApplicationDelegate

- (void)applicationDidFinishLaunching:(NSNotification *)aNotification
{
    amee::Logger::init(amee::LogLevel::Debug);

    [self setupMenuBar];

    // Create platform window
    _window = std::make_unique<MacWindow>();
    _window->create(1024, 768, "AMEE Engine (OpenGL 4.1)");
    _window->setMinSize(320, 240);
    _window->center();
    _window->show();

    // Create GL context on window
    _glContext = std::make_unique<MacGLContext>();
    if (!_glContext->create(_window->getNativeHandle())) {
        AMEE_LOG_ERROR("App", "Failed to create GL context");
        return;
    }
    _glContext->makeCurrent();

    // Create RHI
    _rhi = std::make_unique<RHIOpenGL>();
    [self setupTriangle];

    // Create game loop
    _gameLoop = std::make_unique<MacGameLoop>();
    _gameLoop->start([self](double dt, double time) {
        [self renderFrame:dt time:time];
    });

    AMEE_LOG_INFO("App", "Application launched with OpenGL 4.1");
}

- (void)applicationWillTerminate:(NSNotification *)aNotification
{
    _gameLoop->stop();
    _glContext->makeCurrent();

    _rhi->destroyVertexArray(_triangleVAO);
    _rhi->destroyVertexBuffer(_triangleVBO);
    _triangleShader.reset();
    _rhi.reset();
    _glContext.reset();
    _window.reset();

    AMEE_LOG_INFO("App", "Application terminating");
    amee::Logger::flush();
}

- (BOOL)applicationSupportsSecureRestorableState:(NSApplication *)app
{
    return YES;
}

- (BOOL)applicationShouldTerminateAfterLastWindowClosed:(NSApplication *)sender
{
    return YES;
}

#pragma mark - Setup

- (void)setupMenuBar
{
    NSMenu *mainMenu = [[NSMenu alloc] init];
    [NSApp setMainMenu:mainMenu];

    NSMenuItem *appMenuItem = [[NSMenuItem alloc] init];
    [mainMenu addItem:appMenuItem];
    NSMenu *appMenu = [[NSMenu alloc] initWithTitle:@"AMEE Engine"];
    [appMenuItem setSubmenu:appMenu];

    [appMenu addItemWithTitle:@"About AMEE Engine"
                       action:@selector(orderFrontStandardAboutPanel:)
                keyEquivalent:@""];
    [appMenu addItem:[NSMenuItem separatorItem]];
    [appMenu addItemWithTitle:@"Quit AMEE Engine"
                       action:@selector(terminate:)
                keyEquivalent:@"q"];

    NSMenuItem *windowMenuItem = [[NSMenuItem alloc] init];
    [mainMenu addItem:windowMenuItem];
    NSMenu *windowMenu = [[NSMenu alloc] initWithTitle:@"Window"];
    [windowMenuItem setSubmenu:windowMenu];
    [NSApp setWindowsMenu:windowMenu];

    [windowMenu addItemWithTitle:@"Minimize"
                          action:@selector(performMiniaturize:)
                   keyEquivalent:@"m"];
    [windowMenu addItemWithTitle:@"Zoom"
                          action:@selector(performZoom:)
                   keyEquivalent:@""];
}

- (void)setupTriangle
{
    _triangleShader = std::make_unique<GLShaderProgram>();

    const char* vs = R"(
        #version 410 core
        layout(location = 0) in vec3 aPos;
        layout(location = 1) in vec3 aColor;
        uniform mat4 uMVP;
        out vec3 vColor;
        void main() {
            gl_Position = uMVP * vec4(aPos, 1.0);
            vColor = aColor;
        }
    )";

    const char* fs = R"(
        #version 410 core
        in vec3 vColor;
        out vec4 fragColor;
        void main() {
            fragColor = vec4(vColor, 1.0);
        }
    )";

    _triangleShader->compileFromSource(ShaderType::Vertex, vs, [](const ShaderCompileError& err) {
        AMEE_LOG_ERROR("Shader", "Vertex compile error: %s", err.message.c_str());
    });

    _triangleShader->compileFromSource(ShaderType::Fragment, fs, [](const ShaderCompileError& err) {
        AMEE_LOG_ERROR("Shader", "Fragment compile error: %s", err.message.c_str());
    });

    if (!_triangleShader->link([](const std::string& err) {
        AMEE_LOG_ERROR("Shader", "Link error: %s", err.c_str());
    })) {
        AMEE_LOG_ERROR("Shader", "Failed to create triangle shader");
        return;
    }

    float vertices[] = {
        -0.5f, -0.5f, 0.0f,   1.0f, 0.0f, 0.0f,
         0.5f, -0.5f, 0.0f,   0.0f, 1.0f, 0.0f,
         0.0f,  0.5f, 0.0f,   0.0f, 0.0f, 1.0f,
    };

    _triangleVAO = _rhi->createVertexArray();
    _triangleVBO = _rhi->createVertexBuffer(vertices, sizeof(vertices));

    _rhi->bindVertexArray(_triangleVAO);
    _rhi->bindVertexBuffer(_triangleVBO);

    _rhi->enableVertexAttribArray(0);
    _rhi->vertexAttribPointer(0, 3, RHIDataType::Float, false, 6 * sizeof(float), (void*)0);

    _rhi->enableVertexAttribArray(1);
    _rhi->vertexAttribPointer(1, 3, RHIDataType::Float, false, 6 * sizeof(float), (void*)(3 * sizeof(float)));

    _rhi->bindVertexArray(0);
    _rhi->bindVertexBuffer(0);

    AMEE_LOG_INFO("Renderer", "Triangle shader ready (VAO=%u, VBO=%u)", _triangleVAO, _triangleVBO);
}

#pragma mark - Render

- (void)renderFrame:(double)deltaTime time:(double)time
{
    _glContext->makeCurrent();

    int w, h;
    _glContext->getSize(w, h);
    _rhi->setViewport({0.0f, 0.0f, (float)w, (float)h});

    _rhi->setClearColor(0.15f, 0.15f, 0.2f, 1.0f);
    _rhi->clear();

    float t = (float)time;
    amee::Mat4 model = amee::Mat4::rotateY(t * 45.0f);

    amee::Mat4 view = amee::Mat4::lookAt(
        {0.0f, 1.0f, 3.0f},
        {0.0f, 0.0f, 0.0f},
        {0.0f, 1.0f, 0.0f}
    );

    float aspect = (float)w / (float)h;
    amee::Mat4 proj = amee::Mat4::perspective(45.0f, aspect, 0.1f, 100.0f);

    amee::Mat4 mvp = proj * view * model;

    _triangleShader->use();
    _triangleShader->setMat4("uMVP", mvp.data());

    _rhi->bindVertexArray(_triangleVAO);
    _rhi->drawArrays(RHIPrimitive::Triangles, 3, 0);
    _rhi->bindVertexArray(0);

    _glContext->swapBuffers();
}

@end
