#include "Game.h"
#include "../input/InputManager.h"
#include <iostream>

namespace omegarace {

Game::Game() : running(false), m_AccumulatedTime(0.0), m_LastUpdateTime(0.0) {
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

int Game::onExecute() {
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
    m_LastUpdateTime = pTimer->seconds();
    m_AccumulatedTime = 0.0;

    // BGFX main loop
    while (running && !Window::ShouldClose()) {

        // Process SDL events and update input state FIRST
        Window::BeginFrame();

        handleInput(); // Process input every frame

        // Check for controller connections periodically
        InputManager::updateControllerDetection();

        // Check for fullscreen state changes (e.g., green button clicked)
        if (Window::CheckForFullscreenToggle()) {
            // Screen size changed - update UI components
            pGameController->onScreenSizeChanged();
        }

        // Fixed timestep update at 60fps
        double currentTime = pTimer->seconds();
        double deltaTime = currentTime - m_LastUpdateTime;
        m_LastUpdateTime = currentTime;

        // Cap delta time to prevent spiral of death
        if (deltaTime > 0.25)
            deltaTime = 0.25;

        m_AccumulatedTime += deltaTime;

        // Update game logic at fixed 60fps intervals
        while (m_AccumulatedTime >= TARGET_FRAME_TIME) {
            if (!pTimer->paused()) {
                pGameController->update(TARGET_FRAME_TIME);
            }
            m_AccumulatedTime -= TARGET_FRAME_TIME;
        }

        std::this_thread::yield();

        onRender();
        Window::EndFrame();

        std::this_thread::yield();
    }
    return status;
}

void Game::onUpdate() {
    // This method is no longer used in the main loop
    // Game logic is now updated at fixed 60fps intervals in the main loop
    // Keeping this method for backwards compatibility or debugging purposes
}

void Game::onRender() {
    // Draw neon grid background for Geometry Wars style with player distortion
    // More subtle grid: thinner lines, dimmer colors, lower alpha
    if (pGameController->isPlayerActive()) {
        Vector2f playerPos = pGameController->getPlayerPosition();
        Window::DrawNeonGrid(32.0f, 0.025f, 1.0f, {0, 150, 200, 60}, &playerPos);
    } else {
        // No distortion when player is inactive
        Window::DrawNeonGrid(32.0f, 0.025f, 1.0f, {0, 150, 200, 60}, nullptr);
    }
    
    pGameController->draw();
    
    // Apply aggressive post-process bloom effect for enhanced Geometry Wars-style glow
    // Lower threshold (0.1), much higher intensity (3.5), larger radius (0.05)
    Window::ApplyPostProcessBloom(0.1f, 3.5f, 0.05f);
}

void Game::handleInput() {
    // Handle controller connection/disconnection
    // Raylib automatically handles controller detection, so less complex than SDL

    // Handle fullscreen toggle (F11 key)
    if (InputManager::IsKeyPressed(KEY_F11)) {
        Window::ToggleFullscreen();
    }

    // Handle quit (Escape key)
    if (InputManager::IsKeyPressed(KEY_ESCAPE)) {
        running = false;
    }

    // Pass input to game controller
    pGameController->handleInput();
}

} // namespace omegarace
