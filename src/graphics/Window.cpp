#include "Window.h"
#include "MetalLayerSetup.h"
#include <algorithm>
#include <cctype>
#include <cstring>
#include <filesystem>
#include <fstream>
#include <vector>
#include <cmath>
#include <SDL2/SDL_syswm.h>
#include <bx/math.h>
#include <bx/allocator.h>
#include <bgfx/bgfx.h>

#ifdef __APPLE__
#include <mach-o/dyld.h>
#endif

namespace omegarace {

// Random Number Generator
std::mt19937 m_Random;

// Static members
SDL_Window* Window::mWindow = nullptr;
SDL_Renderer* Window::mRenderer = nullptr;
Rectangle Window::mBox;
int Window::mControllerIndex = -1;
bool Window::mIsFullscreen = false;
int Window::mWindowedWidth = 1024;
int Window::mWindowedHeight = 768;

// BGFX rendering state
bgfx::ViewId Window::mMainView = 0;
bgfx::ViewId Window::mBloomView = 1;
bgfx::ProgramHandle Window::mBloomProgram = BGFX_INVALID_HANDLE;
bgfx::ProgramHandle Window::mLineProgram = BGFX_INVALID_HANDLE;
bgfx::ProgramHandle Window::m_program = BGFX_INVALID_HANDLE;
bgfx::FrameBufferHandle Window::mBloomFrameBuffer = BGFX_INVALID_HANDLE;
bgfx::TextureHandle Window::mBloomTexture = BGFX_INVALID_HANDLE;
bgfx::UniformHandle Window::mBloomParams = BGFX_INVALID_HANDLE;

// Scaling for aspect ratio preservation
float Window::mRenderScale = 1.0f;
Vector2i Window::mRenderOffset = {0, 0};

// Frame timing
std::chrono::high_resolution_clock::time_point Window::mLastFrameTime;
double Window::mDeltaTime = 0.0;

// Input state
bool Window::mShouldClose = false;
bool Window::mKeysPressed[512] = {false};

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
    mWindow = SDL_CreateWindow(
        title.c_str(),
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        width, height,
        SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE
    );

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

    // Try to load shader program, but continue without it if loading fails
    m_program = loadProgram("vs_line", "fs_line");
    if (!bgfx::isValid(m_program)) {
        std::cout << "Note: Custom line shaders not available, using BGFX built-in rendering" << std::endl;
        m_program = BGFX_INVALID_HANDLE;
    }
    
    // Set up controller
    mControllerIndex = findController();
    
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
    std::cout << "InitializeBGFX: Starting BGFX initialization..." << std::endl;
    
    // Use multi-threaded mode with Metal layer to avoid semaphore deadlock
    std::cout << "InitializeBGFX: Setting up multi-threaded mode with Metal layer..." << std::endl;
    
    bgfx::Init init;
    
    // Use Metal renderer for macOS
    init.type = bgfx::RendererType::Metal;
    std::cout << "InitializeBGFX: Using Metal renderer" << std::endl;

    // Get native window handles
    SDL_SysWMinfo wmi;
    SDL_VERSION(&wmi.version);
    if (!SDL_GetWindowWMInfo(mWindow, &wmi)) {
        std::cout << "InitializeBGFX: Failed to get window info" << std::endl;
        return false;
    }

    // Setup Metal layer directly to avoid deadlock
    void* metalLayer = BGFX::cbSetupMetalLayer(wmi.info.cocoa.window);
    std::cout << "InitializeBGFX: Created Metal layer directly" << std::endl;

    bgfx::PlatformData pd;
    pd.nwh = metalLayer;  // Pass Metal layer instead of window
    std::cout << "InitializeBGFX: Set Metal layer handle" << std::endl;

    init.platformData = pd;
    init.resolution.width = mWindowedWidth;
    init.resolution.height = mWindowedHeight;
    init.resolution.reset = BGFX_RESET_VSYNC;
    
    // Configure for multi-threaded operation 
    init.callback = nullptr;
    init.allocator = nullptr;
    
    std::cout << "InitializeBGFX: Calling bgfx::init()..." << std::endl;
    bool result = bgfx::init(init);
    std::cout << "InitializeBGFX: bgfx::init() returned: " << (result ? "SUCCESS" : "FAILED") << std::endl;
        
    return result;
}

void Window::SetupRenderStates() {
    // Configure main view with black background like the original game
    // Use proper RGBA format for BGFX: 0xRRGGBBAA (black = 0x000000FF)
    bgfx::setViewClear(mMainView, BGFX_CLEAR_COLOR | BGFX_CLEAR_DEPTH, 0x000000FF, 1.0f, 0);
    bgfx::setViewRect(mMainView, 0, 0, uint16_t(mWindowedWidth), uint16_t(mWindowedHeight));
    
    // Set up orthographic projection for 2D game coordinates
    float orthoMatrix[16];
    bx::mtxOrtho(orthoMatrix, 0.0f, (float)GAME_WIDTH, (float)GAME_HEIGHT, 0.0f, -1.0f, 1.0f, 0.0f, bgfx::getCaps()->homogeneousDepth);
    bgfx::setViewTransform(mMainView, nullptr, orthoMatrix);
    }

void Window::CreateBloomResources() {
    // Create bloom texture and framebuffer
    mBloomTexture = bgfx::createTexture2D(
        uint16_t(mWindowedWidth), uint16_t(mWindowedHeight), 
        false, 1, bgfx::TextureFormat::RGBA8, 
        BGFX_TEXTURE_RT | BGFX_SAMPLER_U_CLAMP | BGFX_SAMPLER_V_CLAMP
    );
    
    bgfx::TextureHandle bloomAttachments[] = { mBloomTexture };
    mBloomFrameBuffer = bgfx::createFrameBuffer(1, bloomAttachments, true);
    
    // Create bloom uniform
    std::cout << "About to create bloom uniform..." << std::endl;
    mBloomParams = bgfx::createUniform("u_bloomParams", bgfx::UniformType::Vec4);
    std::cout << "Created bloom uniform successfully" << std::endl;
    
    // Try to load volumetric line shader program for bloom effects
    mLineProgram = loadProgram("vs_volumetric_line", "fs_volumetric_line");
    if (!bgfx::isValid(mLineProgram)) {
        std::cout << "Volumetric line shaders not available, falling back to basic line shaders" << std::endl;
        mLineProgram = loadProgram("vs_line", "fs_line");
        if (!bgfx::isValid(mLineProgram)) {
            std::cout << "Basic line shaders not available either, using built-in rendering" << std::endl;
            mLineProgram = BGFX_INVALID_HANDLE;
        }
    } else {
        std::cout << "Successfully loaded volumetric line shaders with bloom support" << std::endl;
    }
}

void Window::Quit() {
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
    
    if (bgfx::isValid(m_program)) {
        bgfx::destroy(m_program);
        m_program = BGFX_INVALID_HANDLE;
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
            std::cout << "Unsupported renderer for shader loading" << std::endl;
            return BGFX_INVALID_HANDLE;
    }
    
    std::string fullPath = basePath + rendererDir + "/" + name + ".bin";
    
    // Try to load shader from file
    std::ifstream file(fullPath, std::ios::binary | std::ios::ate);
    if (!file.is_open()) {
        std::cout << "Failed to open shader file: " << fullPath << std::endl;
        return BGFX_INVALID_HANDLE;
    }
    
    std::streamsize size = file.tellg();
    file.seekg(0, std::ios::beg);
    
    std::vector<char> buffer(size);
    if (!file.read(buffer.data(), size)) {
        std::cout << "Failed to read shader file: " << fullPath << std::endl;
        return BGFX_INVALID_HANDLE;
    }
    
    const bgfx::Memory* mem = bgfx::copy(buffer.data(), uint32_t(size));
    bgfx::ShaderHandle shader = bgfx::createShader(mem);
    
    if (!bgfx::isValid(shader)) {
        std::cout << "Failed to create shader from: " << fullPath << std::endl;
        return BGFX_INVALID_HANDLE;
    }
    
    return shader;
}

bgfx::ProgramHandle Window::loadProgram(const char* vsName, const char* fsName) {
    bgfx::ShaderHandle vs = loadShader(vsName);
    bgfx::ShaderHandle fs = loadShader(fsName);
    
    if (!bgfx::isValid(vs) || !bgfx::isValid(fs)) {
        std::cout << "Failed to load shaders for program: " << vsName << ", " << fsName << std::endl;
        
        // Clean up any valid shaders
        if (bgfx::isValid(vs)) bgfx::destroy(vs);
        if (bgfx::isValid(fs)) bgfx::destroy(fs);
        
        return BGFX_INVALID_HANDLE;
    }
    
    bgfx::ProgramHandle program = bgfx::createProgram(vs, fs, true);
    
    if (!bgfx::isValid(program)) {
        std::cout << "Failed to create shader program from: " << vsName << ", " << fsName << std::endl;
        return BGFX_INVALID_HANDLE;
    }
    
    std::cout << "Successfully loaded shader program: " << vsName << ", " << fsName << std::endl;
    return program;
}

void Window::BeginFrame() {
    // Calculate delta time
    auto currentTime = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(currentTime - mLastFrameTime);
    mDeltaTime = duration.count() / 1000000.0; // Convert to seconds
    mLastFrameTime = currentTime;

    // Store previous keyboard state for key press detection
    static std::vector<Uint8> previousKeyState;
    const Uint8* currentKeyState = SDL_GetKeyboardState(NULL);
    static int numKeys = 0;
    if (numKeys == 0) {
        SDL_GetKeyboardState(&numKeys);
        previousKeyState.resize(numKeys);
    }
    
    // Copy current state to previous for next frame
    std::copy(currentKeyState, currentKeyState + numKeys, previousKeyState.begin());

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
        } else if (event.type == SDL_KEYDOWN) {
            // Store key press events for IsKeyPressed detection
            SDL_Scancode scancode = event.key.keysym.scancode;
            if (scancode < 512) { // SDL_NUM_SCANCODES
                mKeysPressed[scancode] = true;
            }
        }
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

    // Set viewport with letterboxing
    bgfx::setViewRect(mMainView, uint16_t(mRenderOffset.x), uint16_t(mRenderOffset.y), 
                      uint16_t(scaledWidth), uint16_t(scaledHeight));
}

void Window::EndFrame() {
    // For multi-threaded mode, just call frame() - BGFX handles threading
    bgfx::frame();
    
    // Clear key pressed state for next frame
    std::fill(std::begin(mKeysPressed), std::end(mKeysPressed), false);
}

bool Window::ShouldClose() {
    return mShouldClose;
}

void Window::logError(std::ostream& os, const std::string& msg) {
    os << msg << " error: " << SDL_GetError() << std::endl;
}

Rectangle Window::Box() {
    mBox.width = (float)GAME_WIDTH;  // Always return logical game dimensions
    mBox.height = (float)GAME_HEIGHT;
    return mBox;
}

void Window::DrawLine(Line* LineLocation, const Color& LineColor) {
    // For now, use simple line drawing - we'll enhance this with proper BGFX vertex buffers later
    // This is a placeholder implementation
    DrawVolumetricLine(LineLocation, LineColor, 2.0f);
}

void Window::DrawVolumetricLine(Line* LineLocation, const Color& LineColor, float thickness) {
    DrawVolumetricLineWithBloom(LineLocation, LineColor, thickness, 0.5f);
}

void Window::DrawVolumetricLineWithBloom(Line* LineLocation, const Color& LineColor, 
                                        float thickness, float bloomIntensity) {
    if (!LineLocation) return;
        
    // Pack color as 32-bit ABGR for BGFX
    uint32_t color = 
        (uint32_t(LineColor.alpha) << 24) |
        (uint32_t(LineColor.blue) << 16) |
        (uint32_t(LineColor.green) << 8) |
        uint32_t(LineColor.red);
        
    // Calculate line vector and perpendicular for thickness
    float dx = (float)(LineLocation->end.x - LineLocation->start.x);
    float dy = (float)(LineLocation->end.y - LineLocation->start.y);
    float length = sqrtf(dx * dx + dy * dy);
    
    if (length > 0.0f) {
        // Normalize direction vector
        dx /= length;
        dy /= length;
        
        // Calculate perpendicular vector for thickness
        float px = -dy * (thickness * 0.5f);
        float py = dx * (thickness * 0.5f);
        
        // Create quad vertices for thick line with texture coordinates
        struct VolumetricLineVertex {
            float x, y;
            uint32_t color;
            float u, v; // Texture coordinates for distance calculation
        };
        
        VolumetricLineVertex vertices[4] = {
            { (float)LineLocation->start.x + px, (float)LineLocation->start.y + py, color, 0.0f, 0.0f }, // Top-left
            { (float)LineLocation->start.x - px, (float)LineLocation->start.y - py, color, 0.0f, 1.0f }, // Bottom-left
            { (float)LineLocation->end.x - px, (float)LineLocation->end.y - py, color, 1.0f, 1.0f },     // Bottom-right
            { (float)LineLocation->end.x + px, (float)LineLocation->end.y + py, color, 1.0f, 0.0f }      // Top-right
        };
        
        // Index buffer for quad (two triangles)
        uint16_t indices[6] = { 0, 1, 2, 0, 2, 3 };
        
        // Create vertex layout for volumetric lines
        static bgfx::VertexLayout volumetricLayout;
        static bgfx::VertexLayout basicLayout;
        static bool layoutsInitialized = false;
        if (!layoutsInitialized) {
            // Layout for volumetric shaders (with texture coordinates)
            volumetricLayout.begin()
                .add(bgfx::Attrib::Position, 2, bgfx::AttribType::Float)
                .add(bgfx::Attrib::Color0, 4, bgfx::AttribType::Uint8, true)
                .add(bgfx::Attrib::TexCoord0, 2, bgfx::AttribType::Float)
                .end();
                
            // Layout for basic shaders (without texture coordinates)
            basicLayout.begin()
                .add(bgfx::Attrib::Position, 2, bgfx::AttribType::Float)
                .add(bgfx::Attrib::Color0, 4, bgfx::AttribType::Uint8, true)
                .end();
                
            layoutsInitialized = true;
        }
        
        // Choose layout based on shader availability
        bool useVolumetricShader = bgfx::isValid(mLineProgram);
        const bgfx::VertexLayout& layout = useVolumetricShader ? volumetricLayout : basicLayout;
        
        // Set vertex size based on shader type
        struct BasicLineVertex { float x, y; uint32_t color; };
        uint32_t vertexSize = useVolumetricShader ? sizeof(VolumetricLineVertex) : sizeof(BasicLineVertex);
        
        // Allocate transient buffers
        if (bgfx::getAvailTransientVertexBuffer(4, layout) >= 4 &&
            bgfx::getAvailTransientIndexBuffer(6) >= 6) {
            
            bgfx::TransientVertexBuffer tvb;
            bgfx::TransientIndexBuffer tib;
            bgfx::allocTransientVertexBuffer(&tvb, 4, layout);
            bgfx::allocTransientIndexBuffer(&tib, 6);
            
            // Copy vertex data
            if (useVolumetricShader) {
                memcpy(tvb.data, vertices, sizeof(vertices));
            } else {
                // Copy only position and color for basic shaders
                struct BasicVertex { float x, y; uint32_t color; };
                BasicVertex* basicVertices = (BasicVertex*)tvb.data;
                for (int i = 0; i < 4; ++i) {
                    basicVertices[i].x = vertices[i].x;
                    basicVertices[i].y = vertices[i].y;
                    basicVertices[i].color = vertices[i].color;
                }
            }
            
            memcpy(tib.data, indices, sizeof(indices));
            
            // Set buffers
            bgfx::setVertexBuffer(0, &tvb);
            bgfx::setIndexBuffer(&tib);
            
            // Set bloom parameters if using volumetric shader
            if (useVolumetricShader && bgfx::isValid(mBloomParams)) {
                float bloomParams[4] = {
                    bloomIntensity * 1.5f,    // x: bloom intensity (moderate boost)
                    1.2f,                     // y: bloom radius
                    0.5f,                     // z: bloom threshold
                    thickness                 // w: line thickness
                };
                bgfx::setUniform(mBloomParams, bloomParams);
            }
            
            // Set render state for triangles with alpha blending
            uint64_t state = BGFX_STATE_WRITE_RGB | BGFX_STATE_WRITE_A | 
                           BGFX_STATE_BLEND_ALPHA;
            bgfx::setState(state);
            
            // Submit with appropriate shader program
            if (bgfx::isValid(mLineProgram)) {
                bgfx::submit(mMainView, mLineProgram);
            } else {
                // Use BGFX_INVALID_HANDLE to trigger built-in shader
                bgfx::submit(mMainView, BGFX_INVALID_HANDLE);
            }
        }
    }
}

void Window::DrawPoint(Vector2i* Location, const Color& PointColor) {
    if (!Location) return;
    Rectangle rect;
    rect.x = (float)Location->x;
    rect.y = (float)Location->y;
    rect.width = 1.0f;
    rect.height = 1.0f;
    DrawRect(&rect, PointColor);
}

void Window::DrawRect(const Rectangle* RectangleLocation, const Color& RectangleColor) {
    if (!RectangleLocation) return;

    // Pack color as 32-bit ABGR for BGFX
    uint32_t color = 
        (uint32_t(RectangleColor.alpha) << 24) |
        (uint32_t(RectangleColor.blue) << 16) |
        (uint32_t(RectangleColor.green) << 8) |
        uint32_t(RectangleColor.red);

    // Rectangle corners
    float left = RectangleLocation->x;
    float right = RectangleLocation->x + RectangleLocation->width;
    float top = RectangleLocation->y;
    float bottom = RectangleLocation->y + RectangleLocation->height;

    struct LineVertex {
        float x, y;
        uint32_t color;
    };

    // 4 lines: top, right, bottom, left
    LineVertex vertices[8] = {
        { left,  top,    color }, { right, top,    color },    // Top
        { right, top,    color }, { right, bottom, color },    // Right
        { right, bottom, color }, { left,  bottom, color },    // Bottom
        { left,  bottom, color }, { left,  top,    color }     // Left
    };

    static bgfx::VertexLayout layout;
    static bool layoutInitialized = false;
    if (!layoutInitialized) {
        layout.begin()
            .add(bgfx::Attrib::Position, 2, bgfx::AttribType::Float)
            .add(bgfx::Attrib::Color0, 4, bgfx::AttribType::Uint8, true)
            .end();
        layoutInitialized = true;
    }

    if (bgfx::getAvailTransientVertexBuffer(8, layout) >= 8) {
        bgfx::TransientVertexBuffer tvb;
        bgfx::allocTransientVertexBuffer(&tvb, 8, layout);
        memcpy(tvb.data, vertices, sizeof(vertices));
        bgfx::setVertexBuffer(0, &tvb, 0, 8);
        uint64_t state = BGFX_STATE_WRITE_RGB | BGFX_STATE_WRITE_A | BGFX_STATE_BLEND_ALPHA | BGFX_STATE_PT_LINES;
        bgfx::setState(state);
        if (bgfx::isValid(m_program)) {
            bgfx::submit(mMainView, m_program);
        } else {
            bgfx::submit(mMainView, BGFX_INVALID_HANDLE);
        }
    }
}

Vector2i Window::GetWindowSize() {
    return { GAME_WIDTH, GAME_HEIGHT }; // Always return logical game dimensions
}

int Window::Random(int Min, int Max) {
    std::uniform_int_distribution<uint32_t> roll(Min, Max);
    return roll(m_Random);
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

void Window::updateControllerDetection() {
    // Check if current controller is still connected  
    if (mControllerIndex >= 0 && !SDL_GameControllerGetAttached(SDL_GameControllerFromInstanceID(mControllerIndex))) {
        mControllerIndex = -1;
    }
    
    // Check for new controllers periodically
    static auto lastCheck = std::chrono::high_resolution_clock::now();
    auto now = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(now - lastCheck);
    
    if (duration.count() > 500) { // Check every 500ms
        lastCheck = now;
        int newController = findController();
        if (newController != -1 && newController != mControllerIndex) {
            mControllerIndex = newController;
        }
    }
}

int Window::findController() {
    // Look for connected game controllers
    for (int i = 0; i < SDL_NumJoysticks(); i++) {
        if (SDL_IsGameController(i)) {
            SDL_GameController* controller = SDL_GameControllerOpen(i);
            if (controller) {
                const char* name = SDL_GameControllerName(controller);
                if (name) {
                    std::string nameStr = name;
                    std::transform(nameStr.begin(), nameStr.end(), nameStr.begin(), ::tolower);
                    
                    // Check for PlayStation controller patterns
                    if (nameStr.find("ps4") != std::string::npos ||
                        nameStr.find("dualshock") != std::string::npos ||
                        nameStr.find("dualsense") != std::string::npos ||
                        nameStr.find("sony") != std::string::npos ||
                        nameStr.find("wireless controller") != std::string::npos) {
                        // Preferred controller found
                        return i;
                    }
                }
                // Keep first available controller as fallback
                return i;
            }
        }
    }
    return -1;
}

// Controller input functions
bool Window::IsControllerButtonPressed(int button) {
    if (mControllerIndex >= 0) {
        SDL_GameController* controller = SDL_GameControllerFromInstanceID(mControllerIndex);
        if (controller) {
            return SDL_GameControllerGetButton(controller, (SDL_GameControllerButton)button);
        }
    }
    return false;
}

bool Window::IsControllerButtonDown(int button) {
    return IsControllerButtonPressed(button); // Same as pressed for SDL
}

Vector2f Window::GetControllerLeftStick() {
    if (mControllerIndex >= 0) {
        SDL_GameController* controller = SDL_GameControllerFromInstanceID(mControllerIndex);
        if (controller) {
            float x = SDL_GameControllerGetAxis(controller, SDL_CONTROLLER_AXIS_LEFTX) / 32767.0f;
            float y = SDL_GameControllerGetAxis(controller, SDL_CONTROLLER_AXIS_LEFTY) / 32767.0f;
            return {x, y};
        }
    }
    return {0.0f, 0.0f};
}

Vector2f Window::GetControllerRightStick() {
    if (mControllerIndex >= 0) {
        SDL_GameController* controller = SDL_GameControllerFromInstanceID(mControllerIndex);
        if (controller) {
            float x = SDL_GameControllerGetAxis(controller, SDL_CONTROLLER_AXIS_RIGHTX) / 32767.0f;
            float y = SDL_GameControllerGetAxis(controller, SDL_CONTROLLER_AXIS_RIGHTY) / 32767.0f;
            return {x, y};
        }
    }
    return {0.0f, 0.0f};
}

float Window::GetControllerLeftTrigger() {
    if (mControllerIndex >= 0) {
        SDL_GameController* controller = SDL_GameControllerFromInstanceID(mControllerIndex);
        if (controller) {
            return SDL_GameControllerGetAxis(controller, SDL_CONTROLLER_AXIS_TRIGGERLEFT) / 32767.0f;
        }
    }
    return 0.0f;
}

float Window::GetControllerRightTrigger() {
    if (mControllerIndex >= 0) {
        SDL_GameController* controller = SDL_GameControllerFromInstanceID(mControllerIndex);
        if (controller) {
            return SDL_GameControllerGetAxis(controller, SDL_CONTROLLER_AXIS_TRIGGERRIGHT) / 32767.0f;
        }
    }
    return 0.0f;
}

bool Window::IsControllerConnected() {
    return mControllerIndex >= 0;
}

std::string Window::GetControllerName() {
    if (mControllerIndex >= 0) {
        SDL_GameController* controller = SDL_GameControllerFromInstanceID(mControllerIndex);
        if (controller) {
            const char* name = SDL_GameControllerName(controller);
            return name ? std::string(name) : "Unknown Controller";
        }
    }
    return "No Controller";
}

void Window::BeginBloomMode() {
    // TODO: Implement BGFX bloom mode
}

void Window::EndBloomMode() {
    // TODO: Implement BGFX bloom mode 
}

// Full screen warp effect for dramatic wave transitions
void Window::DrawFullScreenWarp(float intensity, float time) {
    // Draw concentric rings and radial lines from center
    const int numRings = 6;
    const int numRadials = 16;
    float cx = GAME_WIDTH * 0.5f;
    float cy = GAME_HEIGHT * 0.5f;
    float maxRadius = std::min(GAME_WIDTH, GAME_HEIGHT) * 0.5f * (0.7f + 0.3f * intensity);
    float ringAlpha = 0.5f * intensity;
    float radialAlpha = 0.3f * intensity;
    float baseHue = fmodf(time * 0.2f, 1.0f);

    // Helper to pack color
    auto packColor = [](float r, float g, float b, float a) -> uint32_t {
        return (uint32_t(a * 255) << 24) |
               (uint32_t(b * 255) << 16) |
               (uint32_t(g * 255) << 8) |
               (uint32_t(r * 255));
    };

    // Draw rings
    for (int i = 1; i <= numRings; ++i) {
        float t = float(i) / (numRings + 1);
        float radius = maxRadius * t * (0.8f + 0.2f * sinf(time + t * 6.28f));
        uint32_t color = packColor(1.0f, 1.0f, 1.0f, ringAlpha * (1.0f - t * 0.5f));
        const int segments = 64;
        struct V { float x, y; uint32_t color; };
        V verts[segments * 2];
        for (int s = 0; s < segments; ++s) {
            float a0 = (s     ) * 2.0f * 3.1415926f / segments;
            float a1 = (s + 1 ) * 2.0f * 3.1415926f / segments;
            verts[s*2+0] = { cx + cosf(a0) * radius, cy + sinf(a0) * radius, color };
            verts[s*2+1] = { cx + cosf(a1) * radius, cy + sinf(a1) * radius, color };
        }
        static bgfx::VertexLayout layout;
        static bool layoutInitialized = false;
        if (!layoutInitialized) {
            layout.begin()
                .add(bgfx::Attrib::Position, 2, bgfx::AttribType::Float)
                .add(bgfx::Attrib::Color0, 4, bgfx::AttribType::Uint8, true)
                .end();
            layoutInitialized = true;
        }
        if (bgfx::getAvailTransientVertexBuffer(segments * 2, layout) >= segments * 2) {
            bgfx::TransientVertexBuffer tvb;
            bgfx::allocTransientVertexBuffer(&tvb, segments * 2, layout);
            memcpy(tvb.data, verts, sizeof(verts));
            bgfx::setVertexBuffer(0, &tvb, 0, segments * 2);
            uint64_t state = BGFX_STATE_WRITE_RGB | BGFX_STATE_WRITE_A | BGFX_STATE_BLEND_ALPHA | BGFX_STATE_PT_LINES;
            bgfx::setState(state);
            if (bgfx::isValid(m_program)) {
                bgfx::submit(mMainView, m_program);
            } else {
                bgfx::submit(mMainView, BGFX_INVALID_HANDLE);
            }
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
        uint32_t color = packColor(1.0f, 1.0f, 1.0f, radialAlpha);
        struct V { float x, y; uint32_t color; };
        V verts[2] = { {x0, y0, color}, {x1, y1, color} };
        static bgfx::VertexLayout layout;
        static bool layoutInitialized = false;
        if (!layoutInitialized) {
            layout.begin()
                .add(bgfx::Attrib::Position, 2, bgfx::AttribType::Float)
                .add(bgfx::Attrib::Color0, 4, bgfx::AttribType::Uint8, true)
                .end();
            layoutInitialized = true;
        }
        if (bgfx::getAvailTransientVertexBuffer(2, layout) >= 2) {
            bgfx::TransientVertexBuffer tvb;
            bgfx::allocTransientVertexBuffer(&tvb, 2, layout);
            memcpy(tvb.data, verts, sizeof(verts));
            bgfx::setVertexBuffer(0, &tvb, 0, 2);
            uint64_t state = BGFX_STATE_WRITE_RGB | BGFX_STATE_WRITE_A | BGFX_STATE_BLEND_ALPHA | BGFX_STATE_PT_LINES;
            bgfx::setState(state);
            if (bgfx::isValid(m_program)) {
                bgfx::submit(mMainView, m_program);
            } else {
                bgfx::submit(mMainView, BGFX_INVALID_HANDLE);
            }
        }
    }
}

// Shader and effects support
void Window::LoadBloomShader(const std::string& fragmentShaderPath) {
    // TODO: Load BGFX shader program for bloom effect
    // This would involve loading and compiling vertex and fragment shaders
    static bool warningShown = false;
    if (!warningShown) {
        std::cout << "Warning: LoadBloomShader not implemented yet for BGFX" << std::endl;
        warningShown = true;
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

// Input functions - Raylib-compatible API using SDL2
bool Window::IsKeyPressed(int key) {
    // Map our key constants to SDL scancodes
    SDL_Scancode scancode;
    switch (key) {
        case KEY_A: scancode = SDL_SCANCODE_A; break;
        case KEY_D: scancode = SDL_SCANCODE_D; break;
        case KEY_W: scancode = SDL_SCANCODE_W; break;
        case KEY_S: scancode = SDL_SCANCODE_S; break;
        case KEY_N: scancode = SDL_SCANCODE_N; break;
        case KEY_P: scancode = SDL_SCANCODE_P; break;
        case KEY_LEFT: scancode = SDL_SCANCODE_LEFT; break;
        case KEY_RIGHT: scancode = SDL_SCANCODE_RIGHT; break;
        case KEY_UP: scancode = SDL_SCANCODE_UP; break;
        case KEY_SPACE: scancode = SDL_SCANCODE_SPACE; break;
        case KEY_LEFT_CONTROL: scancode = SDL_SCANCODE_LCTRL; break;
        case KEY_ESCAPE: scancode = SDL_SCANCODE_ESCAPE; break;
        case KEY_F11: scancode = SDL_SCANCODE_F11; break;
        default: 
            return false;
    }
    
    bool result = mKeysPressed[scancode];
    
    return result;
}

bool Window::IsKeyDown(int key) {
    // Map our key constants to SDL scancodes
    SDL_Scancode scancode;
    switch (key) {
        case KEY_A: scancode = SDL_SCANCODE_A; break;
        case KEY_D: scancode = SDL_SCANCODE_D; break;
        case KEY_W: scancode = SDL_SCANCODE_W; break;
        case KEY_S: scancode = SDL_SCANCODE_S; break;
        case KEY_N: scancode = SDL_SCANCODE_N; break;
        case KEY_P: scancode = SDL_SCANCODE_P; break;
        case KEY_LEFT: scancode = SDL_SCANCODE_LEFT; break;
        case KEY_RIGHT: scancode = SDL_SCANCODE_RIGHT; break;
        case KEY_UP: scancode = SDL_SCANCODE_UP; break;
        case KEY_SPACE: scancode = SDL_SCANCODE_SPACE; break;
        case KEY_LEFT_CONTROL: scancode = SDL_SCANCODE_LCTRL; break;
        case KEY_ESCAPE: scancode = SDL_SCANCODE_ESCAPE; break;
        case KEY_F11: scancode = SDL_SCANCODE_F11; break;
        default: return false;
    }
    
    const Uint8* keyState = SDL_GetKeyboardState(NULL);
    return keyState[scancode];
}

bool Window::IsGamepadButtonPressed(int gamepad, int button) {
    // For now, treat pressed and down the same way
    return IsGamepadButtonDown(gamepad, button);
}

bool Window::IsGamepadButtonDown(int gamepad, int button) {
    if (mControllerIndex < 0) return false;
    
    SDL_GameController* controller = SDL_GameControllerFromInstanceID(mControllerIndex);
    if (!controller) return false;
    
    // Map our button constants to SDL2 constants
    SDL_GameControllerButton sdlButton;
    switch (button) {
        case GAMEPAD_BUTTON_MIDDLE_LEFT: sdlButton = SDL_CONTROLLER_BUTTON_BACK; break;
        case GAMEPAD_BUTTON_MIDDLE_RIGHT: sdlButton = SDL_CONTROLLER_BUTTON_START; break;
        case GAMEPAD_BUTTON_LEFT_FACE_LEFT: sdlButton = SDL_CONTROLLER_BUTTON_DPAD_LEFT; break;
        case GAMEPAD_BUTTON_LEFT_FACE_RIGHT: sdlButton = SDL_CONTROLLER_BUTTON_DPAD_RIGHT; break;
        case GAMEPAD_BUTTON_RIGHT_TRIGGER_1: sdlButton = SDL_CONTROLLER_BUTTON_RIGHTSHOULDER; break;
        default: return false;
    }
    
    return SDL_GameControllerGetButton(controller, sdlButton) != 0;
}

float Window::GetGamepadAxisMovement(int gamepad, int axis) {
    if (mControllerIndex < 0) return 0.0f;
    
    SDL_GameController* controller = SDL_GameControllerFromInstanceID(mControllerIndex);
    if (!controller) return 0.0f;
    
    SDL_GameControllerAxis sdlAxis;
    switch (axis) {
        case GAMEPAD_AXIS_LEFT_X: sdlAxis = SDL_CONTROLLER_AXIS_LEFTX; break;
        default: return 0.0f;
    }
    
    Sint16 value = SDL_GameControllerGetAxis(controller, sdlAxis);
    return value / 32767.0f;  // Normalize to -1.0 to 1.0
}

} // namespace omegarace
