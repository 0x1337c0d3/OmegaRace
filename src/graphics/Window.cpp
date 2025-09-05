#include "Window.h"
#include "MetalLayerSetup.h"
#include "../input/InputManager.h"
#include "../core/GameController.h"
#include "../core/Logger.h"
#include <SDL2/SDL_syswm.h>
#include <algorithm>
#include <bgfx/bgfx.h>
#include <bx/allocator.h>
#include <bx/math.h>
#include <cctype>
#include <chrono>
#include <cmath>
#include <cstring>
#include <filesystem>
#include <fstream>
#include <vector>

#ifdef __APPLE__
#    include <mach-o/dyld.h>
#endif

namespace omegarace {

// Random Number Generator
std::mt19937 m_Random;

// Static members
SDL_Window* Window::mWindow = nullptr;
SDL_Renderer* Window::mRenderer = nullptr;
Rectangle Window::mBox;
bool Window::mIsFullscreen = false;
int Window::mWindowedWidth = 1024;
int Window::mWindowedHeight = 768;

// BGFX rendering state
bgfx::ViewId Window::mBackgroundView = 0;
bgfx::ViewId Window::mMainView = 1;
bgfx::ViewId Window::mBloomView = 2;
bgfx::ProgramHandle Window::mBloomProgram = BGFX_INVALID_HANDLE;
bgfx::ProgramHandle Window::mLineProgram = BGFX_INVALID_HANDLE;
bgfx::ProgramHandle Window::mGridProgram = BGFX_INVALID_HANDLE;
bgfx::ProgramHandle Window::mParticleProgram = BGFX_INVALID_HANDLE;
bgfx::ProgramHandle Window::mShieldProgram = BGFX_INVALID_HANDLE;
bgfx::ProgramHandle Window::mPostProcessProgram = BGFX_INVALID_HANDLE;
bgfx::ProgramHandle Window::mVaporTrailProgram = BGFX_INVALID_HANDLE;
bgfx::ProgramHandle Window::mWarpProgram = BGFX_INVALID_HANDLE;
bgfx::ProgramHandle Window::mElectricBarrierProgram = BGFX_INVALID_HANDLE;
bgfx::FrameBufferHandle Window::mBloomFrameBuffer = BGFX_INVALID_HANDLE;
bgfx::TextureHandle Window::mBloomTexture = BGFX_INVALID_HANDLE;
bgfx::UniformHandle Window::mBloomParams = BGFX_INVALID_HANDLE;
bgfx::UniformHandle Window::mGridParams = BGFX_INVALID_HANDLE;
bgfx::UniformHandle Window::mGridPlayerPos = BGFX_INVALID_HANDLE;
bgfx::UniformHandle Window::mParticleParams = BGFX_INVALID_HANDLE;
bgfx::UniformHandle Window::mShieldParams = BGFX_INVALID_HANDLE;
bgfx::UniformHandle Window::mVaporParams = BGFX_INVALID_HANDLE;
bgfx::UniformHandle Window::mWarpParams = BGFX_INVALID_HANDLE;
bgfx::UniformHandle Window::mElectricParams = BGFX_INVALID_HANDLE;

// Scaling for aspect ratio preservation
float Window::mRenderScale = 1.0f;
Vector2i Window::mRenderOffset = {0, 0};

// Frame timing
std::chrono::high_resolution_clock::time_point Window::mLastFrameTime;
double Window::mDeltaTime = 0.0;

// Input state
bool Window::mShouldClose = false;

bool Window::mShouldQuit = false;

void Window::Init(int width, int height, std::string title) {
    m_Random.seed((unsigned int)time(0));

    // Store initial windowed dimensions
    mWindowedWidth = width;
    mWindowedHeight = height;
    mIsFullscreen = false;

    // Initialize SDL2
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_GAMECONTROLLER) < 0) {
        throw std::runtime_error("Failed to initialize SDL2: " + std::string(SDL_GetError()));
    }

    // Create window
    mWindow = SDL_CreateWindow(title.c_str(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height,
                               SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);

    if (!mWindow) {
        SDL_Quit();
        throw std::runtime_error("Failed to create window: " + std::string(SDL_GetError()));
    }

    // Initialize BGFX (renders into the SDL2 window)
    if (!InitializeBGFX()) {
        SDL_DestroyWindow(mWindow);
        SDL_Quit();
        throw std::runtime_error("Failed to initialize BGFX");
    }

    // Try to load shader programs, but continue without them if loading fails
    mLineProgram = loadProgram("vs_line", "fs_line");
    if (!bgfx::isValid(mLineProgram)) {
        omegarace::Logger::Error("Custom line shaders FAILED to load", "using BGFX built-in rendering");
        mLineProgram = BGFX_INVALID_HANDLE;
    }

    // Initialize InputManager
    InputManager::Init();

    // Set up window box
    mBox.x = 0;
    mBox.y = 0;
    mBox.width = (float)width;
    mBox.height = (float)height;

    // Initialize frame timing
    mLastFrameTime = std::chrono::high_resolution_clock::now();

    // Setup render states and create bloom resources
    SetupRenderStates();
    CreateBloomResources();
}

bool Window::InitializeBGFX() {

    // Use multi-threaded mode with Metal layer to avoid semaphore deadlock


    bgfx::Init init;

    // Use Metal renderer for macOS
    init.type = bgfx::RendererType::Metal;


    // Get native window handles
    SDL_SysWMinfo wmi;
    SDL_VERSION(&wmi.version);
    if (!SDL_GetWindowWMInfo(mWindow, &wmi)) {
        omegarace::Logger::Error("InitializeBGFX: Failed to get window info", "SDL_GetWindowWMInfo failed");
        return false;
    }

    // Setup Metal layer directly to avoid deadlock
    void* metalLayer = BGFX::cbSetupMetalLayer(wmi.info.cocoa.window);

    bgfx::PlatformData pd;
    pd.nwh = metalLayer; // Pass Metal layer instead of window

    init.platformData = pd;
    init.resolution.width = mWindowedWidth;
    init.resolution.height = mWindowedHeight;
    init.resolution.reset = BGFX_RESET_VSYNC;

    // Configure for multi-threaded operation
    init.callback = nullptr;
    init.allocator = nullptr;

    bool result = bgfx::init(init);

    return result;
}

void Window::SetupRenderStates() {
    // Configure background view to clear the entire screen with black
    // This ensures no magenta/purple appears in letterboxed areas
    bgfx::setViewClear(mBackgroundView, BGFX_CLEAR_COLOR, 0x000000FF, 1.0f, 0);
    bgfx::setViewRect(mBackgroundView, 0, 0, uint16_t(mWindowedWidth), uint16_t(mWindowedHeight));
    
    // Set up orthographic projection for background view (for grid rendering)
    float backgroundOrthoMatrix[16];
    bx::mtxOrtho(backgroundOrthoMatrix, 0.0f, (float)GAME_WIDTH, (float)GAME_HEIGHT, 0.0f, -1.0f, 1.0f, 0.0f,
                 bgfx::getCaps()->homogeneousDepth);
    bgfx::setViewTransform(mBackgroundView, nullptr, backgroundOrthoMatrix);

    // Configure main view with black background like the original game
    // Use proper RGBA format for BGFX: 0xRRGGBBAA (black = 0x000000FF)
    bgfx::setViewClear(mMainView, BGFX_CLEAR_COLOR | BGFX_CLEAR_DEPTH, 0x000000FF, 1.0f, 0);
    bgfx::setViewRect(mMainView, 0, 0, uint16_t(mWindowedWidth), uint16_t(mWindowedHeight));

    // Set up orthographic projection for 2D game coordinates
    float orthoMatrix[16];
    bx::mtxOrtho(orthoMatrix, 0.0f, (float)GAME_WIDTH, (float)GAME_HEIGHT, 0.0f, -1.0f, 1.0f, 0.0f,
                 bgfx::getCaps()->homogeneousDepth);
    bgfx::setViewTransform(mMainView, nullptr, orthoMatrix);
}

void Window::CreateBloomResources() {
    // Create bloom texture and framebuffer
    mBloomTexture =
        bgfx::createTexture2D(uint16_t(mWindowedWidth), uint16_t(mWindowedHeight), false, 1, bgfx::TextureFormat::RGBA8,
                              BGFX_TEXTURE_RT | BGFX_SAMPLER_U_CLAMP | BGFX_SAMPLER_V_CLAMP);

    bgfx::TextureHandle bloomAttachments[] = {mBloomTexture};
    mBloomFrameBuffer = bgfx::createFrameBuffer(1, bloomAttachments, true);

    // Create bloom uniform
    mBloomParams = bgfx::createUniform("u_bloomParams", bgfx::UniformType::Vec4);

    // Create additional shader uniforms
    mGridParams = bgfx::createUniform("u_gridParams", bgfx::UniformType::Vec4);
    mGridPlayerPos = bgfx::createUniform("u_gridPlayerPos", bgfx::UniformType::Vec4);
    mParticleParams = bgfx::createUniform("u_particleParams", bgfx::UniformType::Vec4);
    mShieldParams = bgfx::createUniform("u_shieldParams", bgfx::UniformType::Vec4);
    mVaporParams = bgfx::createUniform("u_vaporParams", bgfx::UniformType::Vec4);
    mWarpParams = bgfx::createUniform("u_warpParams", bgfx::UniformType::Vec4);
    mElectricParams = bgfx::createUniform("u_electricParams", bgfx::UniformType::Vec4);

    // Try to load volumetric line shader program for bloom effects
    mBloomProgram = loadProgram("vs_volumetric_line", "fs_volumetric_line");
    if (!bgfx::isValid(mBloomProgram)) {
        omegarace::Logger::Warn("Volumetric line shaders not available, falling back to basic line shaders");
        mBloomProgram = loadProgram("vs_line", "fs_line");
        if (!bgfx::isValid(mBloomProgram)) {
            omegarace::Logger::Error("Basic line shaders not available either", "using built-in rendering");
            mBloomProgram = BGFX_INVALID_HANDLE;
        }
    }

    // Load additional shader programs for enhanced effects
    mGridProgram = loadProgram("vs_grid", "fs_grid");
    mParticleProgram = loadProgram("vs_particle", "fs_particle");
    mShieldProgram = loadProgram("vs_shield", "fs_shield");
    mPostProcessProgram = loadProgram("vs_bloom", "fs_bloom");
    mVaporTrailProgram = loadProgram("vs_vapor_trail", "fs_vapor_trail");
    mWarpProgram = loadProgram("vs_warp", "fs_warp");
    mElectricBarrierProgram = loadProgram("vs_electric_barrier", "fs_electric_barrier");
}

void Window::Quit() {
    InputManager::Shutdown();
    ShutdownBGFX();

    if (mWindow) {
        SDL_DestroyWindow(mWindow);
        mWindow = nullptr;
    }

    SDL_Quit();
}

void Window::ShutdownBGFX() {
    if (bgfx::isValid(mBloomProgram)) {
        bgfx::destroy(mBloomProgram);
        mBloomProgram = BGFX_INVALID_HANDLE;
    }

    if (bgfx::isValid(mLineProgram)) {
        bgfx::destroy(mLineProgram);
        mLineProgram = BGFX_INVALID_HANDLE;
    }

    if (bgfx::isValid(mGridProgram)) {
        bgfx::destroy(mGridProgram);
        mGridProgram = BGFX_INVALID_HANDLE;
    }

    if (bgfx::isValid(mParticleProgram)) {
        bgfx::destroy(mParticleProgram);
        mParticleProgram = BGFX_INVALID_HANDLE;
    }

    if (bgfx::isValid(mShieldProgram)) {
        bgfx::destroy(mShieldProgram);
        mShieldProgram = BGFX_INVALID_HANDLE;
    }

    if (bgfx::isValid(mPostProcessProgram)) {
        bgfx::destroy(mPostProcessProgram);
        mPostProcessProgram = BGFX_INVALID_HANDLE;
    }

    if (bgfx::isValid(mWarpProgram)) {
        bgfx::destroy(mWarpProgram);
        mWarpProgram = BGFX_INVALID_HANDLE;
    }

    if (bgfx::isValid(mElectricBarrierProgram)) {
        bgfx::destroy(mElectricBarrierProgram);
        mElectricBarrierProgram = BGFX_INVALID_HANDLE;
    }

    if (bgfx::isValid(mBloomFrameBuffer)) {
        bgfx::destroy(mBloomFrameBuffer);
        mBloomFrameBuffer = BGFX_INVALID_HANDLE;
    }

    if (bgfx::isValid(mBloomTexture)) {
        bgfx::destroy(mBloomTexture);
        mBloomTexture = BGFX_INVALID_HANDLE;
    }

    if (bgfx::isValid(mBloomParams)) {
        bgfx::destroy(mBloomParams);
        mBloomParams = BGFX_INVALID_HANDLE;
    }

    if (bgfx::isValid(mGridParams)) {
        bgfx::destroy(mGridParams);
        mGridParams = BGFX_INVALID_HANDLE;
    }

    if (bgfx::isValid(mGridPlayerPos)) {
        bgfx::destroy(mGridPlayerPos);
        mGridPlayerPos = BGFX_INVALID_HANDLE;
    }

    if (bgfx::isValid(mParticleParams)) {
        bgfx::destroy(mParticleParams);
        mParticleParams = BGFX_INVALID_HANDLE;
    }

    if (bgfx::isValid(mShieldParams)) {
        bgfx::destroy(mShieldParams);
        mShieldParams = BGFX_INVALID_HANDLE;
    }

    if (bgfx::isValid(mWarpParams)) {
        bgfx::destroy(mWarpParams);
        mWarpParams = BGFX_INVALID_HANDLE;
    }

    if (bgfx::isValid(mElectricParams)) {
        bgfx::destroy(mElectricParams);
        mElectricParams = BGFX_INVALID_HANDLE;
    }

    bgfx::shutdown();
}

// Shader loading implementation
bgfx::ShaderHandle Window::loadShader(const char* name) {
    std::string basePath = dataPath() + "/shaders/";

    // Determine renderer subdirectory based on renderer type
    const bgfx::Caps* caps = bgfx::getCaps();
    const char* rendererDir = "";

    switch (caps->rendererType) {
        case bgfx::RendererType::Direct3D11:
        case bgfx::RendererType::Direct3D12:
            rendererDir = "dx11";
            break;
        case bgfx::RendererType::OpenGL:
        case bgfx::RendererType::OpenGLES:
            rendererDir = "glsl";
            break;
        case bgfx::RendererType::Metal:
            rendererDir = "metal";
            break;
        case bgfx::RendererType::Vulkan:
            rendererDir = "spirv";
            break;
        default:
            omegarace::Logger::Error("Unsupported renderer for shader loading", "unknown renderer type");
            return BGFX_INVALID_HANDLE;
    }

    std::string fullPath = basePath + rendererDir + "/" + name + ".bin";

    // Try to load shader from file
    std::ifstream file(fullPath, std::ios::binary | std::ios::ate);
    if (!file.is_open()) {
        omegarace::Logger::Error("Failed to open shader file", fullPath);
        return BGFX_INVALID_HANDLE;
    }

    std::streamsize size = file.tellg();
    file.seekg(0, std::ios::beg);

    std::vector<char> buffer(size);
    if (!file.read(buffer.data(), size)) {
        omegarace::Logger::Error("Failed to read shader file", fullPath);
        return BGFX_INVALID_HANDLE;
    }

    const bgfx::Memory* mem = bgfx::copy(buffer.data(), uint32_t(size));
    bgfx::ShaderHandle shader = bgfx::createShader(mem);

    if (!bgfx::isValid(shader)) {
        omegarace::Logger::Error("Failed to create shader from", fullPath);
        return BGFX_INVALID_HANDLE;
    }

    return shader;
}

bgfx::ProgramHandle Window::loadProgram(const char* vsName, const char* fsName) {
    bgfx::ShaderHandle vs = loadShader(vsName);
    bgfx::ShaderHandle fs = loadShader(fsName);

    if (!bgfx::isValid(vs) || !bgfx::isValid(fs)) {
        std::string shaderNames = std::string(vsName) + ", " + std::string(fsName);
        omegarace::Logger::Error("Failed to load shaders for program", shaderNames);

        // Clean up any valid shaders
        if (bgfx::isValid(vs))
            bgfx::destroy(vs);
        if (bgfx::isValid(fs))
            bgfx::destroy(fs);

        return BGFX_INVALID_HANDLE;
    }

    bgfx::ProgramHandle program = bgfx::createProgram(vs, fs, true);

    if (!bgfx::isValid(program)) {
        std::string shaderNames = std::string(vsName) + ", " + std::string(fsName);
        omegarace::Logger::Error("Failed to create shader program from", shaderNames);
        return BGFX_INVALID_HANDLE;
    }

    return program;
}

void Window::BeginFrame() {
    // Calculate delta time
    auto currentTime = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(currentTime - mLastFrameTime);
    mDeltaTime = duration.count() / 1000000.0; // Convert to seconds
    mLastFrameTime = currentTime;

    // Handle window events
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT) {
            mShouldClose = true;
        } else if (event.type == SDL_WINDOWEVENT) {
            if (event.window.event == SDL_WINDOWEVENT_SIZE_CHANGED) {
                int newWidth = event.window.data1;
                int newHeight = event.window.data2;
                bgfx::reset(uint32_t(newWidth), uint32_t(newHeight), BGFX_RESET_VSYNC);
                bgfx::setViewRect(mMainView, 0, 0, uint16_t(newWidth), uint16_t(newHeight));

                mBox.width = (float)newWidth;
                mBox.height = (float)newHeight;
            }
        }
        
        // Pass event to InputManager for processing
        InputManager::ProcessEvent(event);
    }

    // Calculate uniform scale to preserve aspect ratio
    int screenWidth, screenHeight;
    SDL_GetWindowSize(mWindow, &screenWidth, &screenHeight);

    float scaleX = (float)screenWidth / GAME_WIDTH;
    float scaleY = (float)screenHeight / GAME_HEIGHT;
    mRenderScale = fmin(scaleX, scaleY);

    // Calculate centering offset
    float scaledWidth = GAME_WIDTH * mRenderScale;
    float scaledHeight = GAME_HEIGHT * mRenderScale;
    mRenderOffset.x = (int)((screenWidth - scaledWidth) / 2.0f);
    mRenderOffset.y = (int)((screenHeight - scaledHeight) / 2.0f);

    // Set background view to cover entire screen (fixes magenta background in fullscreen)
    bgfx::setViewRect(mBackgroundView, 0, 0, uint16_t(screenWidth), uint16_t(screenHeight));
    
    // Set viewport with letterboxing for game content
    bgfx::setViewRect(mMainView, uint16_t(mRenderOffset.x), uint16_t(mRenderOffset.y), uint16_t(scaledWidth),
                      uint16_t(scaledHeight));
}

void Window::EndFrame() {
    // Background view now has content (grid), so don't touch/clear it
    // The grid shader handles the background clearing and drawing
    
    // For multi-threaded mode, just call frame() - BGFX handles threading
    bgfx::frame();

    // Update InputManager for next frame
    InputManager::Update();
}

bool Window::ShouldClose() {
    return mShouldClose;
}

void Window::SetShouldClose(bool shouldClose) {
    mShouldClose = shouldClose;
}

void Window::logError(std::ostream& os, const std::string& msg) {
    os << msg << " error: " << SDL_GetError() << std::endl;
}

Rectangle Window::Box() {
    mBox.width = (float)GAME_WIDTH; // Always return logical game dimensions
    mBox.height = (float)GAME_HEIGHT;
    return mBox;
}

void Window::DrawLine(Line* LineLocation, const Color& LineColor) {
    if (!LineLocation)
        return;

    // Create simple line vertices with float colors for vec4 shader compatibility
    struct BasicLineVertex {
        float x, y;
        float r, g, b, a; // Color as 4 separate floats for vec4 in shader
    };

    // Convert Color to normalized float values
    float r = LineColor.red / 255.0f;
    float g = LineColor.green / 255.0f;
    float b = LineColor.blue / 255.0f;
    float a = LineColor.alpha / 255.0f;

    BasicLineVertex vertices[2] = {
        {(float)LineLocation->start.x, (float)LineLocation->start.y, r, g, b, a},
        {(float)LineLocation->end.x, (float)LineLocation->end.y, r, g, b, a}
    };

    // Create vertex layout for basic lines (position + vec4 color)
    static bgfx::VertexLayout basicLineLayout;
    static bool layoutInitialized = false;
    if (!layoutInitialized) {
        basicLineLayout.begin()
            .add(bgfx::Attrib::Position, 2, bgfx::AttribType::Float)
            .add(bgfx::Attrib::Color0, 4, bgfx::AttribType::Float)
            .end();
        layoutInitialized = true;
    }

    // Allocate transient vertex buffer
    if (bgfx::getAvailTransientVertexBuffer(2, basicLineLayout) >= 2) {
        bgfx::TransientVertexBuffer tvb;
        bgfx::allocTransientVertexBuffer(&tvb, 2, basicLineLayout);
        memcpy(tvb.data, vertices, sizeof(vertices));

        bgfx::setVertexBuffer(0, &tvb);

        uint64_t state = BGFX_STATE_WRITE_RGB | BGFX_STATE_WRITE_A | BGFX_STATE_BLEND_ADD | BGFX_STATE_PT_LINES;
        bgfx::setState(state);
        
        bgfx::submit(mMainView, mLineProgram);
    }
}

void Window::DrawVolumetricLine(Line* LineLocation, const Color& LineColor, float thickness) {
    DrawVolumetricLineWithBloom(LineLocation, LineColor, thickness, 0.5f);
}

void Window::DrawVolumetricLineWithBloom(Line* LineLocation, const Color& LineColor, float thickness,
                                         float bloomIntensity) {
    if (!LineLocation)
        return;

    // Convert Color to normalized float values for vec4 shader compatibility
    float r = LineColor.red / 255.0f;
    float g = LineColor.green / 255.0f;
    float b = LineColor.blue / 255.0f;
    float a = LineColor.alpha / 255.0f;

    // Calculate line vector and perpendicular for thickness
    float dx = (float)(LineLocation->end.x - LineLocation->start.x);
    float dy = (float)(LineLocation->end.y - LineLocation->start.y);
    float length = sqrtf(dx * dx + dy * dy);

    // Handle zero-length lines (render as points/circles for explosion effects)
    if (length == 0.0f) {
        // For zero-length lines, create a small circle/square at the start point
        float radius = thickness * 0.5f;

        // Create quad vertices for circle/point with vec4 colors
        struct VolumetricLineVertex {
            float x, y;
            float r, g, b, a; // Color as 4 separate floats for vec4 in shader
            float u, v; // Texture coordinates for distance calculation
        };

        VolumetricLineVertex vertices[4] = {
            {(float)LineLocation->start.x - radius, (float)LineLocation->start.y - radius, r, g, b, a, 0.0f, 0.0f}, // Top-left
            {(float)LineLocation->start.x - radius, (float)LineLocation->start.y + radius, r, g, b, a, 0.0f, 1.0f}, // Bottom-left
            {(float)LineLocation->start.x + radius, (float)LineLocation->start.y + radius, r, g, b, a, 1.0f, 1.0f}, // Bottom-right
            {(float)LineLocation->start.x + radius, (float)LineLocation->start.y - radius, r, g, b, a, 1.0f, 0.0f}  // Top-right
        };

        // Index buffer for quad (two triangles)
        uint16_t indices[6] = {0, 1, 2, 0, 2, 3};

        // Create vertex layout for volumetric lines with vec4 colors
        static bgfx::VertexLayout volumetricLayout;
        static bool layoutInitialized = false;
        if (!layoutInitialized) {
            volumetricLayout.begin()
                .add(bgfx::Attrib::Position, 2, bgfx::AttribType::Float)
                .add(bgfx::Attrib::Color0, 4, bgfx::AttribType::Float)
                .add(bgfx::Attrib::TexCoord0, 2, bgfx::AttribType::Float)
                .end();
            layoutInitialized = true;
        }

        // Submit the point/circle for rendering
        bgfx::TransientVertexBuffer tvb;
        bgfx::TransientIndexBuffer tib;

        if (bgfx::allocTransientBuffers(&tvb, volumetricLayout, 4, &tib, 6)) {
            memcpy(tvb.data, vertices, sizeof(vertices));
            memcpy(tib.data, indices, sizeof(indices));

            bgfx::setVertexBuffer(0, &tvb);
            bgfx::setIndexBuffer(&tib);
            // Use additive blending for classic vector glow on point explosions
            bgfx::setState(BGFX_STATE_WRITE_RGB | BGFX_STATE_WRITE_A | BGFX_STATE_BLEND_ADD);
            bgfx::submit(mMainView, mBloomProgram);
        }
        return; // Early return for zero-length lines
    }

    if (length > 0.0f) {
        // Normalize direction vector
        dx /= length;
        dy /= length;

        // Calculate perpendicular vector for thickness
        float px = -dy * (thickness * 0.5f);
        float py = dx * (thickness * 0.5f);

        // Create quad vertices for thick line with texture coordinates and vec4 colors
        struct VolumetricLineVertex {
            float x, y;
            float r, g, b, a; // Color as 4 separate floats for vec4 in shader
            float u, v; // Texture coordinates for distance calculation
        };

        VolumetricLineVertex vertices[4] = {
            {(float)LineLocation->start.x + px, (float)LineLocation->start.y + py, r, g, b, a, 0.0f, 0.0f}, // Top-left
            {(float)LineLocation->start.x - px, (float)LineLocation->start.y - py, r, g, b, a, 0.0f, 1.0f}, // Bottom-left
            {(float)LineLocation->end.x - px, (float)LineLocation->end.y - py, r, g, b, a, 1.0f, 1.0f},     // Bottom-right
            {(float)LineLocation->end.x + px, (float)LineLocation->end.y + py, r, g, b, a, 1.0f, 0.0f}      // Top-right
        };

        // Index buffer for quad (two triangles)
        uint16_t indices[6] = {0, 1, 2, 0, 2, 3};

        // Create vertex layout for volumetric lines
        static bgfx::VertexLayout volumetricLayout;
        static bgfx::VertexLayout basicLayout;
        static bool layoutsInitialized = false;
        if (!layoutsInitialized) {
            // Layout for volumetric shaders (with texture coordinates)
            volumetricLayout.begin()
                .add(bgfx::Attrib::Position, 2, bgfx::AttribType::Float)
                .add(bgfx::Attrib::Color0, 4, bgfx::AttribType::Float)
                .add(bgfx::Attrib::TexCoord0, 2, bgfx::AttribType::Float)
                .end();

            // Layout for basic shaders (without texture coordinates)
            basicLayout.begin()
                .add(bgfx::Attrib::Position, 2, bgfx::AttribType::Float)
                .add(bgfx::Attrib::Color0, 4, bgfx::AttribType::Float)
                .end();

            layoutsInitialized = true;
        }

        // Allocate transient buffers
        if (bgfx::getAvailTransientVertexBuffer(4, volumetricLayout) >= 4 && bgfx::getAvailTransientIndexBuffer(6) >= 6) {
            bgfx::TransientVertexBuffer tvb;
            bgfx::TransientIndexBuffer tib;
            bgfx::allocTransientVertexBuffer(&tvb, 4, volumetricLayout);
            bgfx::allocTransientIndexBuffer(&tib, 6);

            // Copy vertex data
            memcpy(tvb.data, vertices, sizeof(vertices));
            memcpy(tib.data, indices, sizeof(indices));

            // Set buffers
            bgfx::setVertexBuffer(0, &tvb);
            bgfx::setIndexBuffer(&tib);

            // Set bloom parameters if using volumetric shader
            float bloomParams[4] = {
                bloomIntensity * 1.5f, // x: bloom intensity (moderate boost)
                1.2f,                  // y: bloom radius
                0.5f,                  // z: bloom threshold
                thickness              // w: line thickness
            };
            bgfx::setUniform(mBloomParams, bloomParams);

            // Set render state for triangles with additive blending for classic vector glow
            uint64_t state = BGFX_STATE_WRITE_RGB | BGFX_STATE_WRITE_A | BGFX_STATE_BLEND_ADD;
            bgfx::setState(state);
            bgfx::submit(mMainView, mBloomProgram);
        }
    }
}

void Window::DrawPoint(Vector2i* Location, const Color& PointColor) {
    if (!Location)
        return;
    Rectangle rect;
    rect.x = (float)Location->x;
    rect.y = (float)Location->y;
    rect.width = 1.0f;
    rect.height = 1.0f;
    // DrawRect(&rect, PointColor);
}

void Window::DrawRect(const Rectangle* RectangleLocation, const Color& RectangleColor) {
    if (!RectangleLocation)
        return;

    // Rectangle corners
    float left = RectangleLocation->x;
    float right = RectangleLocation->x + RectangleLocation->width;
    float top = RectangleLocation->y;
    float bottom = RectangleLocation->y + RectangleLocation->height;

    struct Vertex {
        float x, y;
        float r, g, b, a; // Color as 4 separate floats for vec4 in shader
    };

    static bgfx::VertexLayout layout;
    static bool layoutInitialized = false;
    if (!layoutInitialized) {
        layout.begin()
            .add(bgfx::Attrib::Position, 2, bgfx::AttribType::Float)
            .add(bgfx::Attrib::Color0, 4, bgfx::AttribType::Float)
            .end();
        layoutInitialized = true;
    }

    // Convert Color to normalized float values for vec4 shader compatibility
    float r = RectangleColor.red / 255.0f;
    float g = RectangleColor.green / 255.0f;
    float b = RectangleColor.blue / 255.0f;
    float a = RectangleColor.alpha / 255.0f; // Use actual alpha from input color
    
    // Draw filled rectangle (two triangles) with solid black fill and proper alpha
    Vertex fillVertices[4] = {
                {left, top, 0, 0, 0, a},       // Top-left - black fill with alpha
        {right, top, 0, 0, 0, a},      // Top-right - black fill with alpha
        {left, bottom, 0, 0, 0, a},    // Bottom-left - black fill with alpha
        {right, bottom, 0, 0, 0, a}    // Bottom-right - black fill with alpha
    };
    
    uint16_t fillIndices[6] = {0, 1, 2, 1, 3, 2}; // Two triangles

    if (bgfx::getAvailTransientVertexBuffer(4, layout) >= 4 && bgfx::getAvailTransientIndexBuffer(6) >= 6) {
        bgfx::TransientVertexBuffer fillTvb;
        bgfx::TransientIndexBuffer fillTib;
        bgfx::allocTransientVertexBuffer(&fillTvb, 4, layout);
        bgfx::allocTransientIndexBuffer(&fillTib, 6);
        
        memcpy(fillTvb.data, fillVertices, sizeof(fillVertices));
        memcpy(fillTib.data, fillIndices, sizeof(fillIndices));
        
        bgfx::setVertexBuffer(0, &fillTvb);
        bgfx::setIndexBuffer(&fillTib);
        
        // Enable alpha blending for transparency support
        uint64_t fillState = BGFX_STATE_WRITE_RGB | BGFX_STATE_WRITE_A | BGFX_STATE_DEPTH_TEST_LESS 
                           | BGFX_STATE_BLEND_ALPHA;
        bgfx::setState(fillState);        
        bgfx::submit(mMainView, mLineProgram);
    }

    // Then, draw outline (4 lines: top, right, bottom, left) 
    Vertex outlineVertices[8] = {
        {left, top, r, g, b, a},     {right, top, r, g, b, a},    // Top - with alpha
        {right, top, r, g, b, a},    {right, bottom, r, g, b, a}, // Right - with alpha
        {right, bottom, r, g, b, a}, {left, bottom, r, g, b, a},  // Bottom - with alpha
        {left, bottom, r, g, b, a},  {left, top, r, g, b, a}      // Left - with alpha
    };

    if (bgfx::getAvailTransientVertexBuffer(8, layout) >= 8) {
        bgfx::TransientVertexBuffer outlineTvb;
        bgfx::allocTransientVertexBuffer(&outlineTvb, 8, layout);
        memcpy(outlineTvb.data, outlineVertices, sizeof(outlineVertices));
        bgfx::setVertexBuffer(0, &outlineTvb, 0, 8);
        
        // Lines render with alpha blending enabled
        uint64_t outlineState = BGFX_STATE_WRITE_RGB | BGFX_STATE_WRITE_A | BGFX_STATE_PT_LINES | BGFX_STATE_DEPTH_TEST_LESS 
                              | BGFX_STATE_BLEND_ALPHA;
        bgfx::setState(outlineState);
        bgfx::submit(mMainView, mLineProgram);
    }
}

Vector2i Window::GetWindowSize() {
    return {GAME_WIDTH, GAME_HEIGHT}; // Always return logical game dimensions
}

int Window::Random(int Min, int Max) {
    std::uniform_int_distribution<uint32_t> roll(Min, Max);
    return roll(m_Random);
}

// Enhanced shader-based effects for Geometry Wars style neon aesthetics
void Window::DrawNeonGrid(float gridSize, float lineWidth, float glowIntensity, const Color& gridColor, Vector2f* playerPos) {
    if (!bgfx::isValid(mGridProgram)) {
        return; // Fallback if shaders not available
    }
    
    // Create fullscreen quad for grid rendering
    struct GridVertex {
        float x, y;
        float r, g, b, a;
        float u, v;
    };
    
    float r = gridColor.red / 255.0f;
    float g = gridColor.green / 255.0f;
    float b = gridColor.blue / 255.0f;
    float a = gridColor.alpha / 255.0f;
    
    // Use a quad that covers the logical game area (GAME_WIDTH x GAME_HEIGHT)
    GridVertex vertices[4] = {
        {0.0f, 0.0f, r, g, b, a, 0.0f, 0.0f},                         // Top-left
        {GAME_WIDTH, 0.0f, r, g, b, a, 1.0f, 0.0f},                   // Top-right
        {GAME_WIDTH, GAME_HEIGHT, r, g, b, a, 1.0f, 1.0f},            // Bottom-right
        {0.0f, GAME_HEIGHT, r, g, b, a, 0.0f, 1.0f}                   // Bottom-left
    };
    
    uint16_t indices[6] = {0, 1, 2, 0, 2, 3};
    
    // Create vertex layout for grid rendering
    static bgfx::VertexLayout gridLayout;
    static bool gridLayoutInitialized = false;
    if (!gridLayoutInitialized) {
        gridLayout.begin()
            .add(bgfx::Attrib::Position, 2, bgfx::AttribType::Float)
            .add(bgfx::Attrib::Color0, 4, bgfx::AttribType::Float)
            .add(bgfx::Attrib::TexCoord0, 2, bgfx::AttribType::Float)
            .end();
        gridLayoutInitialized = true;
    }
    
    // Set grid parameters
    static auto startTime = std::chrono::high_resolution_clock::now();
    auto currentTime = std::chrono::high_resolution_clock::now();
    float time = std::chrono::duration<float>(currentTime - startTime).count();
    
    float gridParams[4] = {gridSize, lineWidth, glowIntensity, time};
    if (bgfx::isValid(mGridParams)) {
        bgfx::setUniform(mGridParams, gridParams);
    }
    
    // Set player position for grid distortion effect
    float playerParams[4] = {0.0f, 0.0f, 0.0f, 0.0f}; // Default to no distortion
    if (playerPos != nullptr) {
        playerParams[0] = playerPos->x / (float)GAME_WIDTH;  // Normalize to 0-1
        playerParams[1] = playerPos->y / (float)GAME_HEIGHT; // Normalize to 0-1
        playerParams[2] = 100.0f; // Distortion radius
        playerParams[3] = 0.3f;   // Distortion strength
    }
    if (bgfx::isValid(mGridPlayerPos)) {
        bgfx::setUniform(mGridPlayerPos, playerParams);
    }
    
    // Submit grid rendering
    bgfx::TransientVertexBuffer tvb;
    bgfx::TransientIndexBuffer tib;
    
    if (bgfx::allocTransientBuffers(&tvb, gridLayout, 4, &tib, 6)) {
        memcpy(tvb.data, vertices, sizeof(vertices));
        memcpy(tib.data, indices, sizeof(indices));
        
        bgfx::setVertexBuffer(0, &tvb);
        bgfx::setIndexBuffer(&tib);
        bgfx::setState(BGFX_STATE_WRITE_RGB | BGFX_STATE_WRITE_A | BGFX_STATE_BLEND_ALPHA);
        bgfx::submit(mMainView, mGridProgram);
    }
}

void Window::ResetGridDistortion() {
    // Reset grid distortion by setting distortion strength to 0
    float playerParams[4] = {0.0f, 0.0f, 0.0f, 0.0f}; // No distortion
    if (bgfx::isValid(mGridPlayerPos)) {
        bgfx::setUniform(mGridPlayerPos, playerParams);
    }
}

void Window::DrawParticleEffect(Vector2i* position, float size, float intensity, const Color& particleColor) {
    if (!bgfx::isValid(mParticleProgram)) {
        return; // Fallback if shaders not available
    }
    
    // Create particle quad
    float halfSize = size * 0.5f;
    
    struct ParticleVertex {
        float x, y;
        float r, g, b, a;
        float u, v;
    };
    
    float r = particleColor.red / 255.0f;
    float g = particleColor.green / 255.0f;
    float b = particleColor.blue / 255.0f;
    float a = particleColor.alpha / 255.0f;
    
    ParticleVertex vertices[4] = {
        {position->x - halfSize, position->y - halfSize, r, g, b, a, 0.0f, 0.0f}, // Top-left
        {position->x + halfSize, position->y - halfSize, r, g, b, a, 1.0f, 0.0f}, // Top-right
        {position->x + halfSize, position->y + halfSize, r, g, b, a, 1.0f, 1.0f}, // Bottom-right
        {position->x - halfSize, position->y + halfSize, r, g, b, a, 0.0f, 1.0f}  // Bottom-left
    };
    
    uint16_t indices[6] = {0, 1, 2, 0, 2, 3};
    
    // Create vertex layout for particles
    static bgfx::VertexLayout particleLayout;
    static bool particleLayoutInitialized = false;
    if (!particleLayoutInitialized) {
        particleLayout.begin()
            .add(bgfx::Attrib::Position, 2, bgfx::AttribType::Float)
            .add(bgfx::Attrib::Color0, 4, bgfx::AttribType::Float)
            .add(bgfx::Attrib::TexCoord0, 2, bgfx::AttribType::Float)
            .end();
        particleLayoutInitialized = true;
    }
    
    // Set particle parameters
    static auto startTime = std::chrono::high_resolution_clock::now();
    auto currentTime = std::chrono::high_resolution_clock::now();
    float time = std::chrono::duration<float>(currentTime - startTime).count();
    
    float particleParams[4] = {intensity, 1.0f, time, size}; // fadeType=1.0 for electric spark effect
    bgfx::setUniform(mParticleParams, particleParams);
    
    // Submit particle rendering
    bgfx::TransientVertexBuffer tvb;
    bgfx::TransientIndexBuffer tib;
    
    if (bgfx::allocTransientBuffers(&tvb, particleLayout, 4, &tib, 6)) {
        memcpy(tvb.data, vertices, sizeof(vertices));
        memcpy(tib.data, indices, sizeof(indices));
        
        bgfx::setVertexBuffer(0, &tvb);
        bgfx::setIndexBuffer(&tib);
        bgfx::setState(BGFX_STATE_WRITE_RGB | BGFX_STATE_WRITE_A | BGFX_STATE_BLEND_ADD);
        bgfx::submit(mMainView, mParticleProgram);
    }
}

void Window::DrawShieldGlow(Vector2i* center, float radius, float energy, const Color& shieldColor) {
    if (!bgfx::isValid(mShieldProgram)) {
        return; // Fallback if shaders not available
    }
    
    // Create shield quad
    struct ShieldVertex {
        float x, y;
        float r, g, b, a;
        float u, v;
    };
    
    float r = shieldColor.red / 255.0f;
    float g = shieldColor.green / 255.0f;
    float b = shieldColor.blue / 255.0f;
    float a = shieldColor.alpha / 255.0f;
    
    ShieldVertex vertices[4] = {
        {center->x - radius, center->y - radius, r, g, b, a, 0.0f, 0.0f}, // Top-left
        {center->x + radius, center->y - radius, r, g, b, a, 1.0f, 0.0f}, // Top-right
        {center->x + radius, center->y + radius, r, g, b, a, 1.0f, 1.0f}, // Bottom-right
        {center->x - radius, center->y + radius, r, g, b, a, 0.0f, 1.0f}  // Bottom-left
    };
    
    uint16_t indices[6] = {0, 1, 2, 0, 2, 3};
    
    // Create vertex layout for shield
    static bgfx::VertexLayout shieldLayout;
    static bool shieldLayoutInitialized = false;
    if (!shieldLayoutInitialized) {
        shieldLayout.begin()
            .add(bgfx::Attrib::Position, 2, bgfx::AttribType::Float)
            .add(bgfx::Attrib::Color0, 4, bgfx::AttribType::Float)
            .add(bgfx::Attrib::TexCoord0, 2, bgfx::AttribType::Float)
            .end();
        shieldLayoutInitialized = true;
    }
    
    // Set shield parameters
    static auto startTime = std::chrono::high_resolution_clock::now();
    auto currentTime = std::chrono::high_resolution_clock::now();
    float time = std::chrono::duration<float>(currentTime - startTime).count();
    
    float shieldParams[4] = {energy, time, 1.0f, 0.02f}; // energy, time, distortion, thickness
    bgfx::setUniform(mShieldParams, shieldParams);
    
    // Submit shield rendering
    bgfx::TransientVertexBuffer tvb;
    bgfx::TransientIndexBuffer tib;
    
    if (bgfx::allocTransientBuffers(&tvb, shieldLayout, 4, &tib, 6)) {
        memcpy(tvb.data, vertices, sizeof(vertices));
        memcpy(tib.data, indices, sizeof(indices));
        
        bgfx::setVertexBuffer(0, &tvb);
        bgfx::setIndexBuffer(&tib);
        bgfx::setState(BGFX_STATE_WRITE_RGB | BGFX_STATE_WRITE_A | BGFX_STATE_BLEND_ALPHA);
        bgfx::submit(mMainView, mShieldProgram);
    }
}

void Window::ApplyPostProcessBloom(float threshold, float intensity, float radius) {
    // Set bloom parameters for use in other shaders
    float bloomParams[4] = {threshold, intensity, radius, 16.0f}; // 16 samples
    if (bgfx::isValid(mBloomParams)) {
        bgfx::setUniform(mBloomParams, bloomParams);
    }
    
    // Note: Bloom effect is now applied during normal rendering in volumetric line shaders
    // and other bright element shaders, rather than as a post-process step.
    // This prevents the full-screen white overlay issue.
}

std::string Window::dataPath() {
#ifdef __APPLE__
    // Get the path to the executable on macOS
    char path[1024];
    uint32_t size = sizeof(path);
    if (_NSGetExecutablePath(path, &size) == 0) {
        std::filesystem::path execPath(path);
        std::filesystem::path resourcePath = execPath.parent_path() / ".." / "Resources";
        return resourcePath.string();
    }
#endif

    // Fallback to relative path if unable to get executable path
    return "../Resources";
}



void Window::BeginBloomMode() {
    // TODO: Implement BGFX bloom mode
}

void Window::EndBloomMode() {
    // TODO: Implement BGFX bloom mode
}

// Full screen warp effect for dramatic wave transitions
// Enhanced full screen warp effect using a specialized shader for dramatic neon transitions
void Window::DrawFullScreenWarp(float intensity, float time) {
    // If we have the specialized warp shader, use it for a much better effect
    if (bgfx::isValid(mWarpProgram)) {
        // Create a fullscreen quad that covers the entire logical game area
        struct WarpVertex {
            float x, y;
            float r, g, b, a;
            float u, v; // Texture coordinates for shader effects
        };
        
        // Fullscreen quad vertices covering logical game dimensions
        WarpVertex vertices[4] = {
            {0.0f, 0.0f, 1.0f, 1.0f, 1.0f, intensity, 0.0f, 0.0f},                         // Top-left
            {GAME_WIDTH, 0.0f, 1.0f, 1.0f, 1.0f, intensity, 1.0f, 0.0f},                   // Top-right
            {GAME_WIDTH, GAME_HEIGHT, 1.0f, 1.0f, 1.0f, intensity, 1.0f, 1.0f},            // Bottom-right
            {0.0f, GAME_HEIGHT, 1.0f, 1.0f, 1.0f, intensity, 0.0f, 1.0f}                   // Bottom-left
        };
        
        uint16_t indices[6] = {0, 1, 2, 0, 2, 3};
        
        // Create vertex layout for warp shader
        static bgfx::VertexLayout warpLayout;
        static bool warpLayoutInitialized = false;
        if (!warpLayoutInitialized) {
            warpLayout.begin()
                .add(bgfx::Attrib::Position, 2, bgfx::AttribType::Float)
                .add(bgfx::Attrib::Color0, 4, bgfx::AttribType::Float)
                .add(bgfx::Attrib::TexCoord0, 2, bgfx::AttribType::Float)
                .end();
            warpLayoutInitialized = true;
        }
        
        // Set warp shader parameters for blackhole grid effect
        float warpParams[4] = {
            time,                           // x: Animation time for blackhole dynamics
            intensity * 1.5f,              // y: Effect intensity (moderate boost for visibility)
            2.5f + 1.0f * sinf(time * 0.5f), // z: Blackhole gravitational strength (2-3.5 range)
            8.0f + 2.0f * sinf(time * 0.3f) // w: Grid density (6-10 range for good detail)
        };
        
        if (bgfx::isValid(mWarpParams)) {
            bgfx::setUniform(mWarpParams, warpParams);
        }
        
        // Submit warp effect rendering with additive blending for neon glow
        bgfx::TransientVertexBuffer tvb;
        bgfx::TransientIndexBuffer tib;
        
        if (bgfx::allocTransientBuffers(&tvb, warpLayout, 4, &tib, 6)) {
            memcpy(tvb.data, vertices, sizeof(vertices));
            memcpy(tib.data, indices, sizeof(indices));
            
            bgfx::setVertexBuffer(0, &tvb);
            bgfx::setIndexBuffer(&tib);
            
            // Use additive blending for bright neon effect
            bgfx::setState(BGFX_STATE_WRITE_RGB | BGFX_STATE_WRITE_A | BGFX_STATE_BLEND_ADD);
            bgfx::submit(mMainView, mWarpProgram);
        }
        
        return; // Exit early when using the enhanced shader
    }
    
    // Fallback to original implementation if warp shader is not available
    // Draw concentric rings and radial lines from center
    const int numRings = 6;
    const int numRadials = 16;
    float cx = GAME_WIDTH * 0.5f;
    float cy = GAME_HEIGHT * 0.5f;
    float maxRadius = std::min(GAME_WIDTH, GAME_HEIGHT) * 0.5f * (0.7f + 0.3f * intensity);
    float ringAlpha = 0.5f * intensity;
    float radialAlpha = 0.3f * intensity;

    // Draw rings
    for (int i = 1; i <= numRings; ++i) {
        float t = float(i) / (numRings + 1);
        float radius = maxRadius * t * (0.8f + 0.2f * sinf(time + t * 6.28f));
        float alpha = ringAlpha * (1.0f - t * 0.5f);
        const int segments = 64;
        struct V {
            float x, y;
            float r, g, b, a; // Color as 4 separate floats for vec4 in shader
        };
        V verts[segments * 2];
        for (int s = 0; s < segments; ++s) {
            float a0 = (s) * 2.0f * 3.1415926f / segments;
            float a1 = (s + 1) * 2.0f * 3.1415926f / segments;
            verts[s * 2 + 0] = {cx + cosf(a0) * radius, cy + sinf(a0) * radius, 1.0f, 1.0f, 1.0f, alpha};
            verts[s * 2 + 1] = {cx + cosf(a1) * radius, cy + sinf(a1) * radius, 1.0f, 1.0f, 1.0f, alpha};
        }
        static bgfx::VertexLayout layout;
        static bool layoutInitialized = false;
        if (!layoutInitialized) {
            layout.begin()
                .add(bgfx::Attrib::Position, 2, bgfx::AttribType::Float)
                .add(bgfx::Attrib::Color0, 4, bgfx::AttribType::Float)
                .end();
            layoutInitialized = true;
        }
        if (bgfx::getAvailTransientVertexBuffer(segments * 2, layout) >= segments * 2) {
            bgfx::TransientVertexBuffer tvb;
            bgfx::allocTransientVertexBuffer(&tvb, segments * 2, layout);
            memcpy(tvb.data, verts, sizeof(verts));
            bgfx::setVertexBuffer(0, &tvb, 0, segments * 2);
            uint64_t state = BGFX_STATE_WRITE_RGB | BGFX_STATE_WRITE_A | BGFX_STATE_BLEND_ADD | BGFX_STATE_PT_LINES;
            bgfx::setState(state);
            bgfx::submit(mMainView, mBloomProgram);
        }
    }

    // Draw radials
    for (int i = 0; i < numRadials; ++i) {
        float angle = (float)i / numRadials * 2.0f * 3.1415926f;
        float r0 = maxRadius * 0.2f * (0.8f + 0.2f * sinf(time + i));
        float r1 = maxRadius * (0.95f + 0.05f * cosf(time * 0.7f + i));
        float x0 = cx + cosf(angle) * r0;
        float y0 = cy + sinf(angle) * r0;
        float x1 = cx + cosf(angle) * r1;
        float y1 = cy + sinf(angle) * r1;
        struct V {
            float x, y;
            float r, g, b, a; // Color as 4 separate floats for vec4 in shader
        };
        V verts[2] = {{x0, y0, 1.0f, 1.0f, 1.0f, radialAlpha}, {x1, y1, 1.0f, 1.0f, 1.0f, radialAlpha}};
        static bgfx::VertexLayout layout;
        static bool layoutInitialized = false;
        if (!layoutInitialized) {
            layout.begin()
                .add(bgfx::Attrib::Position, 2, bgfx::AttribType::Float)
                .add(bgfx::Attrib::Color0, 4, bgfx::AttribType::Float)
                .end();
            layoutInitialized = true;
        }
        if (bgfx::getAvailTransientVertexBuffer(2, layout) >= 2) {
            bgfx::TransientVertexBuffer tvb;
            bgfx::allocTransientVertexBuffer(&tvb, 2, layout);
            memcpy(tvb.data, verts, sizeof(verts));
            bgfx::setVertexBuffer(0, &tvb, 0, 2);
            uint64_t state = BGFX_STATE_WRITE_RGB | BGFX_STATE_WRITE_A | BGFX_STATE_BLEND_ADD | BGFX_STATE_PT_LINES;
            bgfx::setState(state);
            bgfx::submit(mMainView, mBloomProgram);
        }
    }
}

void Window::BeginWarpTransition() {
    // TODO: Set up additive blending for warp effect
}

void Window::EndWarpTransition() {
    // TODO: Restore normal blending
}

// Fullscreen support
bool Window::CheckForFullscreenToggle() {
    Uint32 currentFlags = SDL_GetWindowFlags(mWindow);
    bool currentFullscreen = (currentFlags & SDL_WINDOW_FULLSCREEN_DESKTOP) != 0;

    int currentWidth, currentHeight;
    SDL_GetWindowSize(mWindow, &currentWidth, &currentHeight);

    bool stateChanged = (currentFullscreen != mIsFullscreen);
    bool sizeChanged = (currentWidth != (int)mBox.width || currentHeight != (int)mBox.height);

    if (stateChanged || sizeChanged) {
        mIsFullscreen = currentFullscreen;
        mBox.width = (float)currentWidth;
        mBox.height = (float)currentHeight;
        return true;
    }

    return false;
}

void Window::ToggleFullscreen() {
    if (mIsFullscreen) {
        SDL_SetWindowFullscreen(mWindow, 0);
        SDL_SetWindowSize(mWindow, mWindowedWidth, mWindowedHeight);
        SDL_SetWindowPosition(mWindow, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);
    } else {
        SDL_GetWindowSize(mWindow, &mWindowedWidth, &mWindowedHeight);
        SDL_SetWindowFullscreen(mWindow, SDL_WINDOW_FULLSCREEN_DESKTOP);
    }

    mIsFullscreen = !mIsFullscreen;

    // Update window box with new dimensions
    int newWidth, newHeight;
    SDL_GetWindowSize(mWindow, &newWidth, &newHeight);
    mBox.width = (float)newWidth;
    mBox.height = (float)newHeight;
}

bool Window::IsFullscreen() {
    return mIsFullscreen;
}



void Window::DrawVaporTrailSegment(const Vector2f& start, const Vector2f& end, float width, 
                                  float trailPosition, const Color& trailColor, float alpha) {
    if (!bgfx::isValid(mVaporTrailProgram)) {
        return; // Fallback if shader not available
    }
    
    // Get current time for animation
    static auto startTime = std::chrono::high_resolution_clock::now();
    auto currentTime = std::chrono::high_resolution_clock::now();
    float time = std::chrono::duration<float>(currentTime - startTime).count();
    
    // Set vapor trail parameters for much better visibility
    float vaporParams[4] = {
        time,           // x: time for animation
        0.1f,           // y: noise scale (increased from 0.01f)
        0.8f,           // z: turbulence amount (increased from 0.3f)
        0.8f            // w: fade factor (reduced from 1.2f for longer trails)
    };
    if (bgfx::isValid(mVaporParams)) {
        bgfx::setUniform(mVaporParams, vaporParams);
    }
    
    // Calculate direction and perpendicular vectors
    Vector2f direction = {end.x - start.x, end.y - start.y};
    float length = sqrt(direction.x * direction.x + direction.y * direction.y);
    
    if (length < 0.1f) return; // Skip very short segments
    
    // Normalize direction
    direction.x /= length;
    direction.y /= length;
    
    // Calculate perpendicular vector for width
    Vector2f perpendicular = {-direction.y * (width * 0.5f), direction.x * (width * 0.5f)};
    
    // Create quad vertices for vapor trail segment
    struct VaporTrailVertex {
        float x, y;
        float r, g, b, a;
        float u, v; // u = position along trail, v = position across width
    };
    
    float r = trailColor.red / 255.0f;
    float g = trailColor.green / 255.0f;
    float b = trailColor.blue / 255.0f;
    float a_color = (trailColor.alpha / 255.0f) * alpha;
    
    // Create quad with proper texture coordinates for the shader
    VaporTrailVertex vertices[4] = {
        {start.x + perpendicular.x, start.y + perpendicular.y, r, g, b, a_color, trailPosition, 0.0f}, // Top-left
        {start.x - perpendicular.x, start.y - perpendicular.y, r, g, b, a_color, trailPosition, 1.0f}, // Bottom-left
        {end.x - perpendicular.x, end.y - perpendicular.y, r, g, b, a_color, trailPosition + 0.1f, 1.0f}, // Bottom-right
        {end.x + perpendicular.x, end.y + perpendicular.y, r, g, b, a_color, trailPosition + 0.1f, 0.0f}  // Top-right
    };
    
    uint16_t indices[6] = {0, 1, 2, 0, 2, 3};
    
    // Create vertex layout for vapor trails
    static bgfx::VertexLayout vaporLayout;
    static bool layoutInitialized = false;
    if (!layoutInitialized) {
        vaporLayout.begin()
            .add(bgfx::Attrib::Position, 2, bgfx::AttribType::Float)
            .add(bgfx::Attrib::Color0, 4, bgfx::AttribType::Float)
            .add(bgfx::Attrib::TexCoord0, 2, bgfx::AttribType::Float)
            .end();
        layoutInitialized = true;
    }
    
    // Submit vapor trail segment
    bgfx::TransientVertexBuffer tvb;
    bgfx::TransientIndexBuffer tib;
    
    if (bgfx::allocTransientBuffers(&tvb, vaporLayout, 4, &tib, 6)) {
        memcpy(tvb.data, vertices, sizeof(vertices));
        memcpy(tib.data, indices, sizeof(indices));
        
        bgfx::setVertexBuffer(0, &tvb);
        bgfx::setIndexBuffer(&tib);
        bgfx::setState(BGFX_STATE_WRITE_RGB | BGFX_STATE_WRITE_A | BGFX_STATE_BLEND_ALPHA);
        bgfx::submit(mMainView, mVaporTrailProgram);
    }
}

void Window::DrawElectricBarrierLine(Line* LineLocation, const Color& LineColor, 
                                    float pulseSpeed, float thickness, float fadeTime) {
    if (!LineLocation || !bgfx::isValid(mElectricBarrierProgram))
        return;

    // Convert Color to normalized float values
    float r = LineColor.red / 255.0f;
    float g = LineColor.green / 255.0f;
    float b = LineColor.blue / 255.0f;
    float a = LineColor.alpha / 255.0f;

    // Calculate line vector and perpendicular for thickness
    float dx = (float)(LineLocation->end.x - LineLocation->start.x);
    float dy = (float)(LineLocation->end.y - LineLocation->start.y);
    float length = sqrtf(dx * dx + dy * dy);

    if (length == 0.0f)
        return; // Skip zero-length lines for electric barriers

    // Normalize direction vector
    float dirX = dx / length;
    float dirY = dy / length;

    // Calculate perpendicular vector for line thickness
    float perpX = -dirY * thickness * 0.5f;
    float perpY = dirX * thickness * 0.5f;

    // Create electric barrier line vertices
    struct ElectricBarrierVertex {
        float x, y;
        float r, g, b, a;
        float u, v; // u: progress along line (0-1), v: perpendicular distance (-1 to 1)
    };

    ElectricBarrierVertex vertices[4] = {
        // Bottom-left
        {(float)LineLocation->start.x - perpX, (float)LineLocation->start.y - perpY, 
         r, g, b, a, 0.0f, -1.0f},
        // Bottom-right  
        {(float)LineLocation->end.x - perpX, (float)LineLocation->end.y - perpY,
         r, g, b, a, 1.0f, -1.0f},
        // Top-right
        {(float)LineLocation->end.x + perpX, (float)LineLocation->end.y + perpY,
         r, g, b, a, 1.0f, 1.0f},
        // Top-left
        {(float)LineLocation->start.x + perpX, (float)LineLocation->start.y + perpY,
         r, g, b, a, 0.0f, 1.0f}
    };

    uint16_t indices[6] = {0, 1, 2, 0, 2, 3};

    // Create vertex layout for electric barrier shader (same as basic line)
    bgfx::VertexLayout electricLayout;
    electricLayout.begin()
        .add(bgfx::Attrib::Position, 2, bgfx::AttribType::Float)
        .add(bgfx::Attrib::Color0, 4, bgfx::AttribType::Float)
        .add(bgfx::Attrib::TexCoord0, 2, bgfx::AttribType::Float)
        .end();

    // Allocate transient buffers
    bgfx::TransientVertexBuffer tvb;
    bgfx::TransientIndexBuffer tib;

    if (bgfx::allocTransientBuffers(&tvb, electricLayout, 4, &tib, 6)) {
        
        // Copy vertex data
        memcpy(tvb.data, vertices, sizeof(vertices));
        memcpy(tib.data, indices, sizeof(indices));

        // Set buffers
        bgfx::setVertexBuffer(0, &tvb);
        bgfx::setIndexBuffer(&tib);

        // Calculate time for animation (same pattern as other shaders)
        static auto startTime = std::chrono::high_resolution_clock::now();
        auto currentTime = std::chrono::high_resolution_clock::now();
        float time = std::chrono::duration<float>(currentTime - startTime).count();

        // Set electric barrier parameters
        float electricParams[4] = {
            time,       // x: current time for animation
            pulseSpeed, // y: pulse speed (default 15.0)
            thickness,  // z: line thickness
            fadeTime    // w: fade time remaining
        };
        bgfx::setUniform(mElectricParams, electricParams);

        // Set render state with additive blending for electric glow
        uint64_t state = BGFX_STATE_WRITE_RGB | BGFX_STATE_WRITE_A | BGFX_STATE_BLEND_ADD;
        bgfx::setState(state);
        bgfx::submit(mMainView, mElectricBarrierProgram);
    }
}


} // namespace omegarace
