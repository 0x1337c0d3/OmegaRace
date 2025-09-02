#include "Game.h"
#include <iostream>

namespace omegarace {

Game::Game() : running(false) {
    std::cout << "Game constructor called" << std::endl;
}

Game::~Game() {
    onCleanup();
}

int Game::onInit() {
    std::cout << "Game::onInit() called" << std::endl;
    int screenWidth = 1024;
    int screenHeight = 768;

    try {
        std::cout << "Initializing Window..." << std::endl;
        Window::Init(screenWidth, screenHeight, "Omega Race");
        std::cout << "Window initialized successfully" << std::endl;
    } catch (const std::runtime_error& error) {
        Window::logError(std::cout, "Window OnInit error: " + std::string(error.what()));
        Window::Quit();
        return APP_FAILED;
    }

    return APP_OK;
}

void Game::onCleanup() {
}

int Game::OnExecute() {
    std::cout << "Game::OnExecute() called" << std::endl;
    
    int status = onInit();
    if (status != APP_OK) {
        std::cout << "onInit() failed with status: " << status << std::endl;
        return status;
    }
    std::cout << "onInit() completed successfully" << std::endl;

    pTimer = std::make_unique<Timer>();
    pGameController = std::make_unique<GameController>();
    std::cout << "Created Timer and GameController" << std::endl;
    
    pGameController->initialize();
    std::cout << "GameController initialized" << std::endl;
    
    pTimer->start();
    std::cout << "Timer started" << std::endl;

    running = true;
    std::cout << "Entering main loop" << std::endl;

    m_LastTickTime = pTimer->ticks();

    // BGFX main loop
    int frameCount = 0;
    while (running && !Window::ShouldClose()) {
        frameCount++;
        
        // Process SDL events and update input state FIRST
        Window::BeginFrame();
        
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
        
        onRender();
        Window::EndFrame();
                
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
    pGameController->draw();
}

void Game::handleInput() {
    // Handle controller connection/disconnection
    // Raylib automatically handles controller detection, so less complex than SDL
    
    // Handle pause toggle (P key)
    if (Window::IsKeyPressed(KEY_P)) {
        pGameController->onPause(pTimer->paused());
        
        if (pTimer->paused())
            pTimer->unpause();
        else
            pTimer->pause();
    }
    
    // Handle fullscreen toggle (F11 key)
    if (Window::IsKeyPressed(KEY_F11)) {
        Window::ToggleFullscreen();
    }
    
    // Handle quit (Escape key)
    if (Window::IsKeyPressed(KEY_ESCAPE)) {
        running = false;
    }
    
    // Pass input to game controller
    pGameController->handleInput();
}

} // namespace omegarace
