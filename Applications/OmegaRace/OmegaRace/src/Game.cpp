#include "Game.h"

namespace omegarace {

Game::Game() : running(false) {
}

Game::~Game() {
    onCleanup();
}

int Game::onInit() {
    int screenWidth = 1024;
    int screenHeight = 768;

    try {
        Window::Init(screenWidth, screenHeight, "Omega Race");
    } catch (const std::runtime_error& error) {
        Window::logRaylibError(std::cout, "Window OnInit error code: " + std::string(error.what()));
        Window::Quit();
        return APP_FAILED;
    }

    return APP_OK;
}

void Game::onCleanup() {
}

int Game::OnExecute() {
    int status = onInit();
    if (status != APP_OK) {
        return status;
    }

    pTimer = std::make_unique<Timer>();
    pGameController = std::make_unique<GameController>();
    pGameController->initialize();
    pTimer->start();

    running = true;

    m_LastTickTime = pTimer->ticks();

    // Raylib main loop - no need for SDL event polling
    while (running && !Window::ShouldClose()) {
        handleInput();  // Process input every frame
        
        // Check for controller connections periodically
        Window::updateControllerDetection();
        
        // Check for fullscreen state changes (e.g., green button clicked)
        if (Window::CheckForFullscreenToggle()) {
            // Screen size changed - update UI components
            pGameController->onScreenSizeChanged();
        }
        
        onUpdate();
        std::this_thread::yield();
        
        Window::BeginDrawing();
        onRender();
        Window::EndDrawing();
        
        // Optional: Print FPS to console every second
        Window::PrintFPS();
        
        std::this_thread::yield();
    }

    return status;
}

void Game::onUpdate() {
    double frame;
    int ticksThisFrame = pTimer->ticks() - m_LastTickTime;
    frame = (double)ticksThisFrame * 0.001;

    if (!pTimer->paused()) {
        pGameController->update(frame);
    }
    m_LastTickTime = pTimer->ticks();
}

void Game::onRender() {
    Window::Clear();
    pGameController->draw();
    
    // Display FPS counter in top-left corner
    Window::DrawFPS(10, 10);
    
    // Display PS4/PS5 controller status
    if (Window::IsControllerConnected()) {
        std::string controllerInfo = "Controller: " + Window::GetControllerName();
        ::DrawText(controllerInfo.c_str(), 10, 30, 16, (::Color){0, 228, 48, 255}); // Green
        
        // Show if it's a PS4 or PS5 controller specifically
        std::string name = Window::GetControllerName();
        if (name.find("PS4") != std::string::npos ||
            name.find("DualShock") != std::string::npos) {
            ::DrawText("PS4 DualShock Ready!", 10, 50, 16, (::Color){253, 249, 0, 255}); // Yellow
        } else if (name.find("DualSense") != std::string::npos) {
            ::DrawText("PS5 DualSense Ready!", 10, 50, 16, (::Color){0, 162, 232, 255}); // PS5 Blue
        } else if (name.find("Sony") != std::string::npos ||
                   name.find("Wireless Controller") != std::string::npos) {
            ::DrawText("PlayStation Controller Ready!", 10, 50, 16, (::Color){253, 249, 0, 255}); // Yellow
        }
    } else {
        ::DrawText("No Controller Connected", 10, 30, 16, (::Color){230, 41, 55, 255}); // Red
    }
    
    // Note: Window::Present() is not needed - EndDrawing() handles this
}

void Game::handleInput() {
    // Handle controller connection/disconnection
    // Raylib automatically handles controller detection, so less complex than SDL
    
    // Handle pause toggle (P key)
    if (IsKeyPressed(KEY_P)) {
        pGameController->onPause(pTimer->paused());
        
        if (pTimer->paused())
            pTimer->unpause();
        else
            pTimer->pause();
    }
    
    // Handle quit (Escape key)
    if (IsKeyPressed(KEY_ESCAPE)) {
        running = false;
    }
    
    // Pass input to game controller
    pGameController->handleInput();
}

} // namespace omegarace
