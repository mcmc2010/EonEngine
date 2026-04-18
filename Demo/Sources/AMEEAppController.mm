#import "AMEEAppController.h"
#import "AMEEGameLoop.h"
#include <memory>
#include "../AMEEFramework/Render/RHI.h"
#include "../AMEEFramework/Render/GL/RHIOpenGL.h"
#include "../AMEEFramework/Render/Shader/GL/GLShader.h"
#include "../AMEEFramework/Core/Math/AMEEMath.h"
#include "../AMEEFramework/Core/Log/AMEELog.h"

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wdeprecated-declarations"
#import "../AMEEFramework/Platform/macOS/GLView.h"
#pragma clang diagnostic pop

@interface AMEEAppController () <AMEEGameLoopDelegate>
@end

AMEEAppController* _AMEEAppController = nil;

AMEEAppController* GetAppController()
{
    return _AMEEAppController;
}

@implementation AMEEAppController {
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
    [self setupMainWindow];

    _rhi = std::make_unique<RHIOpenGL>();

    [self.glView makeGLContextCurrent];
    [self setupTriangle];

    self.gameLoop = [[AMEEGameLoop alloc] initWithDelegate:self];
    [self.gameLoop start];

    AMEE_LOG_INFO("App", "Application launched with OpenGL 4.1");
}

- (void)applicationWillTerminate:(NSNotification *)aNotification
{
    [self.gameLoop stop];
    [self.glView makeGLContextCurrent];

    _rhi->destroyVertexArray(_triangleVAO);
    _rhi->destroyVertexBuffer(_triangleVBO);
    _triangleShader.reset();
    _rhi.reset();

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

- (void)setupMainWindow
{
    NSRect contentRect = NSMakeRect(0, 0, 1024, 768);
    self.glView = [[GLView alloc] initWithFrame:contentRect];

    NSUInteger styleMask = NSWindowStyleMaskTitled
        | NSWindowStyleMaskClosable
        | NSWindowStyleMaskMiniaturizable
        | NSWindowStyleMaskResizable;

    self.mainWindow = [[NSWindow alloc] initWithContentRect:contentRect
                                                 styleMask:styleMask
                                                   backing:NSBackingStoreBuffered
                                                     defer:NO];
    [self.mainWindow setContentView:self.glView];
    self.mainWindow.title = @"AMEE Engine (OpenGL 4.1)";
    self.mainWindow.minSize = NSMakeSize(320, 240);
    [self.mainWindow center];
    [self.mainWindow makeKeyAndOrderFront:nil];
    
    AMEE_LOG_INFO("Window", "Main window created (%dx%d)", 1024, 768);
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

    // vertex data: x, y, z, r, g, b
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

#pragma mark - AMEEGameLoopDelegate

- (void)gameLoopUpdate:(double)deltaTime time:(double)time
{
}

- (void)gameLoopRender:(double)deltaTime time:(double)time
{
    [self.glView makeGLContextCurrent];

    NSSize size = [self.glView drawableSize];
    _rhi->setViewport({0.0f, 0.0f, (float)size.width, (float)size.height});

    _rhi->setClearColor(0.15f, 0.15f, 0.2f, 1.0f);
    _rhi->clear();

    // 3D rendering with MVP matrix
    float t = (float)time;

    // Model: rotate around Y axis
    amee::Mat4 model = amee::Mat4::rotateY(t * 45.0f);

    // View: camera at (0, 1, 3) looking at origin
    amee::Mat4 view = amee::Mat4::lookAt(
        {0.0f, 1.0f, 3.0f},   // eye
        {0.0f, 0.0f, 0.0f},   // center
        {0.0f, 1.0f, 0.0f}    // up
    );

    // Projection: perspective
    float aspect = size.width / size.height;
    amee::Mat4 proj = amee::Mat4::perspective(45.0f, aspect, 0.1f, 100.0f);

    // MVP
    amee::Mat4 mvp = proj * view * model;

    _triangleShader->use();
    _triangleShader->setMat4("uMVP", mvp.data());

    _rhi->bindVertexArray(_triangleVAO);
    _rhi->drawArrays(RHIPrimitive::Triangles, 3, 0);
    _rhi->bindVertexArray(0);

    _rhi->present();
}

@end
