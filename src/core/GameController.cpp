#include "GameController.h"
#include <ctime>

namespace omegarace {

static StatusDisplay::GAME_STATE states[3] = {
    StatusDisplay::APP_START,
    StatusDisplay::APP_INSTRUCTIONS,
    StatusDisplay::APP_GAMEOVER
};

GameController::GameController() {
    pThePlayer = std::make_unique<Player>();
    pTheBorders = std::make_unique<Borders>();
    pTheEnemyController = std::make_unique<EnemyController>();
    pStatus = std::make_unique<StatusDisplay>();
    
    m_Score = 0;
    m_PlayerShips = 4; // Start with 4 extra lives (5 total including current life)
    m_EndOfWave = false;
    m_State = -1;
    m_CurrentWave = 1; // Initialize wave counter

    m_TimerAmount = 90;
    pTimer = std::make_unique<Timer>();
    pTimer->start();
    m_Respawn = false;
    
    // Initialize random generator for Rock creation
    m_RandomGenerator.seed(static_cast<unsigned int>(time(nullptr)));
    
    // Initialize warp transition system
    m_WarpActive = false;
    m_WarpDuration = 2.0f;
    m_WarpStartTime = 0.0f;
    m_IsFirstWave = true;
    m_WaitingForWarp = false;
    
    // Initialize UFO system
    m_UFO = new UFO();
    m_UFOSpawnTimer = 0.0f;
    m_UFOSpawnInterval = 15.0f; // UFO spawns every 15 seconds
}

GameController::~GameController() {
    // Clean up rocks
    for (omegarace::Rock* rock : m_Rocks) {
        delete rock;
    }
    m_Rocks.clear();
    
    // Clean up UFO
    delete m_UFO;
    
    AudioEngine::Shutdown();
}

void GameController::initialize() {
    AudioEngine::Init();

    pTheBorders->initialize();
    pThePlayer->initialize();
    pThePlayer->setInsideBorder(pTheBorders->getInsideBorder());
    pTheEnemyController->setInisdeBorder(pTheBorders->getInsideBorder());
    pFighter = pTheEnemyController->getFighterPointer();
    pFollower = pTheEnemyController->getFollowPointer();
    pLeader = pTheEnemyController->getLeadPointer();
    pStatus->initialize();

    audio.LoadBank("Master.bank", 0);
    audio.LoadBank("Master.strings.bank", 0);
    audio.PlayEvent("event:/Horizontal Crime Demo");

    audio.LoadSound("EnemyHit");
    audio.LoadSound("EnemyHit");
    audio.LoadSound("LeadEnemyHit");
    audio.LoadSound("FollowerHit");
    audio.LoadSound("FighterHit");
    audio.LoadSound("MineHit");
    audio.LoadSound("PlayerHit");
    
    // Initialize rock system (sound will be handled through FMOD like other sounds)
}

void GameController::update(double Frame) {
    AudioEngine::Update();
    
    // Only update gameplay entities if not during warp transition
    if (!m_WarpActive) {
        pTheBorders->update();
        pThePlayer->update(Frame);
        pTheEnemyController->update(Frame);
        updateRocks(Frame); // Update rocks
        updateUFO(Frame); // Update UFO
        pFighter->setPlayerLocation(pThePlayer->getLocation());
        pLeader->setPlayerLocation(pThePlayer->getLocation());
        pTheEnemyController->setPlayerPosition(pThePlayer->getLocation()); // For menacing FollowEnemy effects
        pStatus->setScore(m_Score);

        if (pThePlayer->getActive()) {
            for (int line = 0; line < 4; line++) {
                if (pThePlayer->getInsideLineHit(line))
                    pTheBorders->setInsideLineHit(line);

                if (pFighter->getActive()) {
                    if (pFighter->getInsideLineHit(line))
                        pTheBorders->setInsideLineHit(line);
                }
            }

            for (int line = 0; line < 8; line++) {
                if (pThePlayer->getOutsideLineHit(line))
                    pTheBorders->setOutsideLineHit(line);

                if (pFighter->getActive()) {
                    if (pFighter->getOutsideLineHit(line))
                        pTheBorders->setOutsideLineHit(line);
                }
            }
        }
        
        checkCollisions();
    }

    if (m_EndOfWave) {
        if (!pTheEnemyController->checkExploding()) {
            if (!m_WaitingForWarp) {
                // First time wave ends - clean up and trigger warp
                completeWaveCleanup(); // NEW: Clean up all remaining entities
                triggerWarpTransition(1.8f); // Trigger warp effect
                m_WaitingForWarp = true; // Set flag to wait for warp completion
            } else if (!m_WarpActive) {
                // Warp has completed - now spawn the new wave
                spawnNewWave(pTheEnemyController->newWave());
                m_EndOfWave = false;
                m_WaitingForWarp = false;
            }
            // If m_WarpActive is still true, just wait for it to complete
        }
    } else if (m_Respawn) {
        if (m_Timer < pTimer->seconds()) {
            spawnNewWave(pTheEnemyController->restartWave());
            m_Respawn = false;
        }
    }

    if (!pThePlayer->getActive()) {
        if (m_Timer < pTimer->seconds()) {
            m_State++;
            if (m_State > 2)
                m_State = 0;
            pStatus->setState(states[m_State]);
            m_Timer = pTimer->seconds() + m_TimerAmount * 0.05;
        }
    }
}

void GameController::draw() {
    // Draw the Geometry Wars grid background first (behind everything)
    Vector2f playerPos = pThePlayer->getLocation();
    
    // Create distortion sources for grid warping
    std::vector<DistortionSource> distortionSources;
    
    // Always add player as primary distortion source
    distortionSources.push_back(DistortionSource(playerPos, 1.0f));
    
    // Add Fighter distortion if active - more intense warping effect
    if (pFighter && pFighter->getActive()) {
        Vector2f fighterPos = pFighter->getLocation();
        // Fighter creates stronger distortion with larger radius for more dramatic effect
        distortionSources.push_back(DistortionSource(fighterPos, 1.5f, 100.0f));
    }
    
    pTheBorders->drawBackground(distortionSources);
    
    // Only draw player if warp transition is not active
    if (!m_WarpActive) {
        pThePlayer->draw();
    }
    pTheEnemyController->draw();
    drawRocks(); // Draw rocks
    drawUFO(); // Draw UFO
    pTheBorders->draw();
    pStatus->draw();
    
    // Draw full screen warp transition effect if active
    if (m_WarpActive) {
        float elapsedTime = pTimer->seconds() - m_WarpStartTime;
        float warpProgress = elapsedTime / m_WarpDuration;
        
        if (warpProgress <= 1.0f) {
            // Create an intensity curve that starts high, peaks in middle, then fades
            float intensity;
            if (warpProgress < 0.5f) {
                intensity = warpProgress * 2.0f; // Ramp up
            } else {
                intensity = (1.0f - warpProgress) * 2.0f; // Ramp down
            }
            
            Window::BeginWarpTransition();
            Window::DrawFullScreenWarp(intensity, elapsedTime * 2.0f);
            Window::EndWarpTransition();
        } else {
            m_WarpActive = false; // Effect finished
        }
    }
}

void GameController::newGame() {
    triggerWarpTransition(2.5f); // Longer warp for new game
    m_IsFirstWave = true; // Reset flag for new game
    m_WaitingForWarp = false; // Reset wave waiting flag for new game
    
    // Clear existing rocks
    for (omegarace::Rock* rock : m_Rocks) {
        delete rock;
    }
    m_Rocks.clear();
    m_CurrentWave = 1; // Reset wave counter
    
    // Reset UFO system
    if (m_UFO) {
        m_UFO->setActive(false);
        m_UFO->setDestroyed(false);
    }
    m_UFOSpawnTimer = 0.0f;
    m_UFOSpawnInterval = 15.0f;
    
    // Reset all entity states to ensure clean start
    resetAllEntityStates();
    
    pThePlayer->newGame();
    pStatus->newGame();
    spawnNewWave(pTheEnemyController->newGame());
    m_Score = 0;
    m_PlayerShips = 4;
    pStatus->setShip(m_PlayerShips);
    m_EndOfWave = false;
    audio.PlayEvent("event:/Horizontal Souls");
}

void GameController::handleInput() {
    // Combined keyboard and controller input handling
    // Start with keyboard states
    bool turnLeft = false;
    bool turnRight = false;
    bool thrust = false;
    
    // === KEYBOARD INPUT ===
    // Turn left (A key or Left arrow)
    if (IsKeyDown(KEY_A) || IsKeyDown(KEY_LEFT)) {
        turnLeft = true;
    }
    
    // Turn right (D key or Right arrow)
    if (IsKeyDown(KEY_D) || IsKeyDown(KEY_RIGHT)) {
        turnRight = true;
    }
    
    // Thrust (W key or Up arrow)
    if (IsKeyDown(KEY_W) || IsKeyDown(KEY_UP)) {
        thrust = true;
    }
    
    // Fire (S key, Space, or Left Ctrl) - use IsKeyPressed for single shot
    if (IsKeyPressed(KEY_S) || IsKeyPressed(KEY_SPACE) || IsKeyPressed(KEY_LEFT_CONTROL)) {
        pThePlayer->fireButtonPressed();
    }
    
    // New game (N key)
    if (IsKeyPressed(KEY_N)) {
        newGame();
    }
    
    // === CONTROLLER INPUT (ADDITIVE) ===
    // PS4 Controller input handling with enhanced support
    if (Window::mControllerIndex >= 0) {
        int gamepadId = Window::mControllerIndex;
        
        // === TURNING CONTROLS ===
        // Method 1: Left analog stick (preferred for smooth control)
        float leftStickX = GetGamepadAxisMovement(gamepadId, GAMEPAD_AXIS_LEFT_X);
        const float STICK_DEADZONE = 0.2f;
        
        // Analog stick turning (smooth)
        if (leftStickX < -STICK_DEADZONE) {
            turnLeft = true;  // Add to keyboard input
        } else if (leftStickX > STICK_DEADZONE) {
            turnRight = true;  // Add to keyboard input
        }
        
        // Method 2: D-pad for precise turning (PS4 D-pad)
        if (IsGamepadButtonDown(gamepadId, GAMEPAD_BUTTON_LEFT_FACE_LEFT)) {  // D-pad Left
            turnLeft = true;  // Add to keyboard input
        }
        if (IsGamepadButtonDown(gamepadId, GAMEPAD_BUTTON_LEFT_FACE_RIGHT)) {  // D-pad Right
            turnRight = true;  // Add to keyboard input
        }
        
        // === THRUST CONTROLS ===
        // Method 1: R1 button - PRIMARY
        if (IsGamepadButtonDown(gamepadId, GAMEPAD_BUTTON_RIGHT_TRIGGER_1)) {  // R1 button
            thrust = true;  // Add to keyboard input
        }
        
        // Method 2: Right analog stick up
        float rightStickY = GetGamepadAxisMovement(gamepadId, GAMEPAD_AXIS_RIGHT_Y);
        if (rightStickY < -STICK_DEADZONE) {  // Up on right stick
            thrust = true;  // Add to keyboard input
        }
                
        // === FIRE CONTROLS ===
        // Method 1: X button (PS4 X = bottom face button) - PRIMARY
        if (IsGamepadButtonPressed(gamepadId, GAMEPAD_BUTTON_RIGHT_FACE_DOWN)) {  // X button
            pThePlayer->fireButtonPressed();
        }
        
        // Method 2: Square button (PS4 Square = left face button)
        if (IsGamepadButtonPressed(gamepadId, GAMEPAD_BUTTON_RIGHT_FACE_LEFT)) {  // Square
            pThePlayer->fireButtonPressed();
        }
        
        // Method 3: Circle button (PS4 Circle = right face button)  
        if (IsGamepadButtonDown(gamepadId, GAMEPAD_BUTTON_RIGHT_FACE_RIGHT)) {  // Circle
            thrust = true;  // Add to keyboard input
        }
                
        // === GAME CONTROLS ===
        // Start button (Options) for new game
        if (IsGamepadButtonPressed(gamepadId, GAMEPAD_BUTTON_MIDDLE_RIGHT)) {  // Options/Start
            newGame();
        }
        
        // Share button for pause (alternative)
        if (IsGamepadButtonPressed(gamepadId, GAMEPAD_BUTTON_MIDDLE_LEFT)) {  // Share/Select
            // Could add pause functionality here if needed
        }
        
        // Triangle button for special actions
        if (IsGamepadButtonPressed(gamepadId, GAMEPAD_BUTTON_RIGHT_FACE_UP)) {  // Triangle
            // Reserved for future features
        }
    }
    
    // Apply combined input states (keyboard OR controller)
    pThePlayer->setTurnLeft(turnLeft);
    pThePlayer->setTurnRight(turnRight);
    pThePlayer->setThrust(thrust);
}

void GameController::onPause(bool paused) {
    if (paused) {
        pTheEnemyController->timerUnpause();
        pThePlayer->timerUnpause();
    } else {
        pTheEnemyController->timerPause();
        pThePlayer->timerPause();
    }
}

void GameController::checkCollisions() {
    for (int shot = 0; shot < pThePlayer->getNumberOfShots(); shot++) {
        if (pThePlayer->getShotActive(shot)) {
            if (doesPlayerShotEnemy(shot)) {
                audio.PlaySoundFile("EnemyHit");
                m_Score += 1000;
            }

            if (doesPlayerShootLeadEnemy(shot)) {
                audio.PlaySoundFile("LeadEnemyHit");
                m_Score += 1500;
            }

            if (doesPlayerShootFollowEnemy(shot)) {
                audio.PlaySoundFile("FollowerHit");
                m_Score += 1500;
            }

            if (doesPlayerShootFighter(shot)) {
                audio.PlaySoundFile("FighterHit");
                m_Score += 2500;
            }

            if (doesPlayerShootFollowMine(shot)) {
                audio.PlaySoundFile("MineHit");
                m_Score += 350;
            }

            if (doesPlayerShootFighterMine(shot)) {
                audio.PlaySoundFile("MineHit");
                m_Score += 500;
            }
            
            // Check rock shooting
            if (doesPlayerShootRock(shot)) {
                audio.PlaySoundFile("MineHit");
                m_Score += 750;
            }
            
            // Check UFO shooting
            if (doesPlayerShootUFO(shot)) {
                audio.PlaySoundFile("Bonus");
                // Score is already added in doesPlayerShootUFO method
            }
        }
    }

    if (pThePlayer->getActive()) {
        for (int ship = 0; ship < pTheEnemyController->getEnemyCount(); ship++) {
            if (pTheEnemyController->getEnemyActive(ship)) {
                if (doesEnemyCollideWithPlayer(ship)) {
                    audio.PlaySoundFile("PlayerHit");
                    if (playerHit()) {
                        break; // Exit collision checking since player is respawning
                    }
                }
            }
        }

        if (pLeader->getActive()) {
            if (doesLeadCollideWithPlayer()) {
                audio.PlaySoundFile("PlayerHit");
                if (playerHit()) {
                    return; // Exit collision checking since player is respawning
                }
            }
        }

        if (pFollower->getActive()) {
            if (doesFollowCollideWithPlayer()) {
                audio.PlaySoundFile("PlayerHit");
                if (playerHit()) {
                    return; // Exit collision checking since player is respawning
                }
            }
        }

        if (pFighter->getActive()) {
            if (doesFighterCollideWithPlayer()) {
                audio.PlaySoundFile("PlayerHit");
                if (playerHit()) {
                    return; // Exit collision checking since player is respawning
                }
            }
        }

        if (pFollower->getMineCount() > 0) {
            for (int mine = 0; mine < pFollower->getMineCount(); mine++) {
                if (pFollower->getMineActive(mine)) {
                    if (doesFollowMineHitPalyer(mine)) {
                        audio.PlaySoundFile("PlayerHit");
                        if (playerHit()) {
                            return; // Exit collision checking since player is respawning
                        }
                    }
                }
            }
        }

        if (pFighter->getMineCount() > 0) {
            for (int mine = 0; mine < pFighter->getMineCount(); mine++) {
                if (pFighter->getMineActive(mine)) {
                    if (doesFighterMineHitPlayer(mine)) {
                        audio.PlaySoundFile("PlayerHit");
                        if (playerHit()) {
                            return; // Exit collision checking since player is respawning
                        }
                    }
                }
            }
        }

        if (pLeader->getShotActive()) {
            if (doesLeadShootPlayer()) {
                audio.PlaySoundFile("PlayerHit");
                if (playerHit()) {
                    return; // Exit collision checking since player is respawning
                }
            }
        }

        if (pFighter->getShotActive()) {
            if (doesFighterShootPlayer()) {
                audio.PlaySoundFile("PlayerHit");
                if (playerHit()) {
                    return; // Exit collision checking since player is respawning
                }
            }
        }
        
        // Check rock collisions with player
        for (int rock = 0; rock < (int)m_Rocks.size(); rock++) {
            if (m_Rocks[rock] && !m_Rocks[rock]->isDestroyed()) {
                if (doesRockCollideWithPlayer(rock)) {
                    // Destroy the rock to prevent multiple hits
                    m_Rocks[rock]->setDestroyed(true);
                    m_Rocks[rock]->triggerDustExplosion();
                    audio.PlaySoundFile("PlayerHit");
                    if (playerHit()) {
                        // Player was hit and still has lives remaining
                        break; // Exit collision checking since player is respawning
                    }
                }
            }
        }
        
        // Check UFO collision with player
        if (doesUFOCollideWithPlayer()) {
            // Destroy the UFO to prevent multiple hits
            m_UFO->triggerExplosion();
            audio.PlaySoundFile("PlayerHit");
            if (playerHit()) {
                // Player was hit and still has lives remaining
                // Exit collision checking since player is respawning
            }
        }
    }
}

bool GameController::doesPlayerShotEnemy(int shot) {
    for (int ship = 0; ship < pTheEnemyController->getEnemyCount(); ship++) {
        if (pTheEnemyController->getEnemyActive(ship)) {
            // Get radius because it wont work inside of other function for unknown reason.
            float radius = pTheEnemyController->getEnemyRadius();

            if (pThePlayer->getShotCircle(pTheEnemyController->getEnemyLocaiton(ship), radius, shot)) {
                pTheEnemyController->enemyHit(ship);
                pThePlayer->setShotActive(shot, false);
                return true;
            }
        }
    }

    return false;
}

bool GameController::doesPlayerShootLeadEnemy(int shot) {
    if (pLeader->getActive()) {
        float radius = pLeader->getRadius();

        if (pThePlayer->getShotCircle(pLeader->getLocation(), radius, shot)) {
            pTheEnemyController->leadEnemyHit();
            pThePlayer->setShotActive(shot, false);

            if (pTheEnemyController->checkEndOfWave()) {
                m_EndOfWave = true;
            }

            return true;
        }
    }

    return false;
}

bool GameController::doesPlayerShootFollowEnemy(int shot) {
    if (pFollower->getActive()) {
        float radius = pFollower->getRadius();

        if (pThePlayer->getShotCircle(pFollower->getLocation(), radius, shot)) {
            pTheEnemyController->followEnemyHit();
            pThePlayer->setShotActive(shot, false);
            return true;
        }
    }

    return false;
}

bool GameController::doesPlayerShootFighter(int shot) {
    if (pFighter->getActive()) {
        float radius = pFighter->getRadius();

        if (pThePlayer->getShotCircle(pFighter->getLocation(), radius, shot)) {
            pFighter->explode();
            pThePlayer->setShotActive(shot, false);

            if (pTheEnemyController->checkEndOfWave()) {
                m_EndOfWave = true;
            }

            return true;
        }
    }

    return false;
}

bool GameController::doesPlayerShootFollowMine(int shot) {
    if (pFollower->getMineCount() > 0) {
        float mineRadius = pFollower->getMineRadius();

        for (int mine = 0; mine < pFollower->getMineCount(); mine++) {
            if (pFollower->getMineActive(mine)) {
                if (pThePlayer->getShotCircle(pFollower->getMineLocaiton(mine), mineRadius, shot)) {
                    pFollower->mineHit(mine);
                    pThePlayer->setShotActive(shot, false);
                    return true;
                }
            }
        }
    }

    return false;
}

bool GameController::doesPlayerShootFighterMine(int shot) {
    int mineCount = pFighter->getMineCount();

    if (mineCount > 0) {
        float mineRadius = pFighter->getMineRadius();

        for (int mine = 0; mine < mineCount; mine++) {
            if (pFighter->getMineActive(mine)) {
                if (pThePlayer->getShotCircle(pFighter->getMineLocaiton(mine), mineRadius, shot)) {
                    pFighter->mineHit(mine);
                    pThePlayer->setShotActive(shot, false);
                    return true;
                }
            }
        }
    }

    return false;
}

bool GameController::doesEnemyCollideWithPlayer(int ship) {
    float radius = pTheEnemyController->getEnemyRadius();

    if (pThePlayer->circlesIntersect(pTheEnemyController->getEnemyLocaiton(ship), radius)) {
        pTheEnemyController->enemyHit(ship);

        if (playerHit())
            m_Respawn = true;

        return true;
    }

    return false;
}

bool GameController::doesLeadCollideWithPlayer() {
    float radius = pLeader->getRadius();

    if (pThePlayer->circlesIntersect(pLeader->getLocation(), radius)) {
        pTheEnemyController->leadEnemyHit();

        if (playerHit())
            m_Respawn = true;

        return true;
    }

    return false;
}

bool GameController::doesFollowCollideWithPlayer() {
    float radius = pFollower->getRadius();

    if (pThePlayer->circlesIntersect(pFollower->getLocation(), radius)) {
        pTheEnemyController->followEnemyHit();

        if (playerHit())
            m_Respawn = true;

        return true;
    }

    return false;
}

bool GameController::doesFighterCollideWithPlayer() {
    float radius = pFighter->getRadius();

    if (pThePlayer->circlesIntersect(pFighter->getLocation(), radius)) {
        pFighter->explode();

        if (playerHit())
            m_Respawn = true;

        return true;
    }

    return false;
}

bool GameController::doesFollowMineHitPalyer(int mine) {
    float radius = pFollower->getMineRadius();

    if (pThePlayer->circlesIntersect(pFollower->getMineLocaiton(mine), radius)) {
        pFollower->mineHit(mine);

        if (playerHit())
            m_Respawn = true;

        return true;
    }

    return false;
}

bool GameController::doesFighterMineHitPlayer(int mine) {
    float radius = pFighter->getMineRadius();

    if (pThePlayer->circlesIntersect(pFighter->getMineLocaiton(mine), radius)) {
        pFighter->mineHit(mine);

        if (playerHit())
            m_Respawn = true;

        return true;
    }

    return false;
}

bool GameController::doesLeadShootPlayer() {
    float radius = pLeader->getShotRadius();

    if (pThePlayer->circlesIntersect(pLeader->getShotLocation(), radius)) {
        pLeader->shotHitTarget();

        if (playerHit())
            m_Respawn = true;

        return true;
    }

    return false;
}

bool GameController::doesFighterShootPlayer() {
    float radius = pFighter->getShotRadius();

    if (pThePlayer->circlesIntersect(pFighter->getShotLocation(), radius)) {
        pFighter->shotHitTarget();

        if (playerHit())
            m_Respawn = true;

        return true;
    }

    return false;
}

void GameController::spawnNewWave(int ships) {
    // Note: Warp transition is now handled in the update() method before calling this
    m_IsFirstWave = false;
        
    bool rightSide;

    if (Window::Random(0, 1))
        rightSide = true;
    else
        rightSide = false;

    pThePlayer->spawn(rightSide);
    pTheEnemyController->spawnNewWave(rightSide, ships);
    
    // Spawn rocks on later waves
    spawnRocks(m_CurrentWave);

    m_CurrentWave++;
}

bool GameController::playerHit() {
    m_PlayerShips--;

    pStatus->setShip(m_PlayerShips);
    if (m_PlayerShips > 0) {
        pThePlayer->hit();
        m_Timer = pTimer->seconds() + m_TimerAmount * 0.01;
        m_Respawn = true;
        return true;
    } else {
        pThePlayer->setActive(false);
        pStatus->setState(StatusDisplay::APP_GAMEOVER);
        m_State = 1;
    }

    return false;
}

void GameController::triggerWarpTransition(float duration) {
    m_WarpActive = true;
    m_WarpDuration = duration;
    m_WarpStartTime = pTimer->seconds();
}

void GameController::spawnRocks(int waveNumber) {
    // Spawn rocks starting from wave 3
    if (waveNumber < 3) {
        return;
    }
    
    // Number of rocks increases with wave number
    int numRocks = (waveNumber - 2) * 2; // Wave 3: 2 rocks, Wave 4: 4 rocks, etc.
    if (numRocks > 8) {
        numRocks = 8; // Cap at 8 rocks
    }
    
    for (int i = 0; i < numRocks; i++) {
        omegarace::Rock* pRock = new omegarace::Rock(m_RandomGenerator);
        
        // Spawn rocks at random positions around the edge of the play area
        omegarace::Vector2f spawnLocation;
        omegarace::Vector2f spawnVelocity;
        int edge = rand() % 4; // 0: top, 1: right, 2: bottom, 3: left
        
        switch (edge) {
            case 0: // Top edge
                spawnLocation.x = (float)(rand() % 1024);
                spawnLocation.y = 50.0f;
                spawnVelocity.x = (float)((rand() % 20) - 10); // -10 to 10
                spawnVelocity.y = (float)(rand() % 10 + 5); // 5 to 15
                break;
            case 1: // Right edge
                spawnLocation.x = 974.0f;
                spawnLocation.y = (float)(rand() % 768);
                spawnVelocity.x = -(float)(rand() % 10 + 5); // -15 to -5
                spawnVelocity.y = (float)((rand() % 20) - 10); // -10 to 10
                break;
            case 2: // Bottom edge
                spawnLocation.x = (float)(rand() % 1024);
                spawnLocation.y = 718.0f;
                spawnVelocity.x = (float)((rand() % 20) - 10); // -10 to 10
                spawnVelocity.y = -(float)(rand() % 10 + 5); // -15 to -5
                break;
            case 3: // Left edge
                spawnLocation.x = 50.0f;
                spawnLocation.y = (float)(rand() % 768);
                spawnVelocity.x = (float)(rand() % 10 + 5); // 5 to 15
                spawnVelocity.y = (float)((rand() % 20) - 10); // -10 to 10
                break;
        }
        
        pRock->activate(spawnLocation, spawnVelocity);
        m_Rocks.push_back(pRock);
    }
}

void GameController::updateRocks(double frame) {
    for (int i = (int)m_Rocks.size() - 1; i >= 0; i--) {
        if (m_Rocks[i]) {
            m_Rocks[i]->update(frame);
            
            // Remove destroyed rocks
            if (m_Rocks[i]->isDestroyed() && !m_Rocks[i]->isDustActive()) {
                delete m_Rocks[i];
                m_Rocks.erase(m_Rocks.begin() + i);
            }
        }
    }
}

void GameController::drawRocks() {
    for (size_t i = 0; i < m_Rocks.size(); i++) {
        if (m_Rocks[i]) {
            // Always draw rocks that are active OR have active dust explosions
            if (!m_Rocks[i]->isDestroyed() || m_Rocks[i]->isDustActive()) {
                m_Rocks[i]->draw();
            }
        }
    }
}

bool GameController::doesPlayerShootRock(int shot) {
    for (size_t rock = 0; rock < m_Rocks.size(); rock++) {
        if (m_Rocks[rock] && m_Rocks[rock]->active && !m_Rocks[rock]->destroyed) {
            // Use SimpleRock properties for collision detection
            omegarace::Vector2f rockLocation = m_Rocks[rock]->position;
            float rockRadius = 20.0f; // SimpleRock collision radius
            
            if (pThePlayer->getShotCircle(rockLocation, rockRadius, shot)) {
                // Rock is destroyed - use proper method to synchronize all variables
                m_Rocks[rock]->setDestroyed(true);
                m_Rocks[rock]->triggerDustExplosion();
                
                // Play explosion sound through FMOD system
                // audio.PlaySoundFile("RockHit");
                
                return true;
            }
        }
    }
    return false;
}

bool GameController::doesRockCollideWithPlayer(int rockIndex) {
    if (rockIndex < 0 || rockIndex >= (int)m_Rocks.size() || !m_Rocks[rockIndex]) {
        return false;
    }
    
    omegarace::Rock* pRock = m_Rocks[rockIndex];
    if (!pRock->active || pRock->destroyed) {
        return false;
    }
    
    // Use Rock properties for collision detection
    omegarace::Vector2f rockLocation = pRock->position;
    float rockRadius = 20.0f; // Rock collision radius
    
    return pThePlayer->circlesIntersect(rockLocation, rockRadius);
}

// UFO system methods
void GameController::spawnUFO(int waveNumber) {
    // Spawn UFO starting from wave 4
    if (waveNumber < 4 || m_UFO->isActive()) {
        return;
    }
    
    // Determine spawn side (random)
    bool fromLeft = (rand() % 2) == 0;
    
    // Spawn location
    omegarace::Vector2f spawnLocation;
    if (fromLeft) {
        spawnLocation.x = -50.0f; // Off left edge
    } else {
        spawnLocation.x = (float)omegarace::Window::GetWindowSize().x + 50.0f; // Off right edge
    }
    spawnLocation.y = 100.0f + (rand() % 400); // Random vertical position
    
    m_UFO->activate(spawnLocation, fromLeft);
}

void GameController::updateUFO(double frame) {
    if (!m_UFO) return;
    
    // Update UFO spawn timer
    m_UFOSpawnTimer += frame;
    
    // Try to spawn UFO periodically
    if (m_UFOSpawnTimer >= m_UFOSpawnInterval && !m_UFO->isActive()) {
        spawnUFO(m_CurrentWave);
        m_UFOSpawnTimer = 0.0f;
        // Randomize next spawn interval
        m_UFOSpawnInterval = 10.0f + ((rand() % 100) * 0.1f); // 10-20 seconds
    }
    
    // Update UFO if active
    if (m_UFO->isActive()) {
        m_UFO->update(frame);
    }
}

void GameController::drawUFO() {
    if (m_UFO && (m_UFO->isActive() || m_UFO->isExplosionActive())) {
        m_UFO->draw(); // This now handles both UFO and explosion drawing
    }
}

bool GameController::doesPlayerShootUFO(int shot) {
    if (!m_UFO || !m_UFO->isActive() || m_UFO->isDestroyed()) {
        return false;
    }
    
    omegarace::Vector2f ufoLocation = m_UFO->getLocation();
    float ufoRadius = m_UFO->getRadius();
    
    if (pThePlayer->getShotCircle(ufoLocation, ufoRadius, shot)) {
        // UFO is destroyed - trigger spectacular explosion
        m_UFO->setDestroyed(true);
        m_UFO->setActive(false);
        m_UFO->triggerExplosion(); // Trigger custom UFO explosion
        
        // Award points for UFO destruction (high value)
        m_Score += 1500;
        
        // Play explosion sound through FMOD system
        // audio.PlaySoundFile("UFOHit");
        
        return true;
    }
    return false;
}

bool GameController::doesUFOCollideWithPlayer() {
    if (!m_UFO || !m_UFO->isActive() || m_UFO->isDestroyed()) {
        return false;
    }
    
    omegarace::Vector2f ufoLocation = m_UFO->getLocation();
    float ufoRadius = m_UFO->getRadius();
    
    return pThePlayer->circlesIntersect(ufoLocation, ufoRadius);
}

void GameController::onScreenSizeChanged() {
    // Reinitialize UI components that depend on screen size
    pTheBorders->initialize();
    
    pStatus->initialize();
    
    // Update inside border for player and enemies
    pThePlayer->setInsideBorder(pTheBorders->getInsideBorder());
    pTheEnemyController->setInisdeBorder(pTheBorders->getInsideBorder());
}

void GameController::completeWaveCleanup() {
    // Destroy all remaining active rocks with spectacular dust explosions
    for (size_t i = 0; i < m_Rocks.size(); i++) {
        if (m_Rocks[i] && !m_Rocks[i]->isDestroyed()) {
            m_Rocks[i]->setDestroyed(true);
            m_Rocks[i]->triggerDustExplosion();
            
            // Award small bonus points for automatic rock destruction
            m_Score += 100;
        }
    }
    
    // Destroy UFO if active with explosion
    if (m_UFO && m_UFO->isActive() && !m_UFO->isDestroyed()) {
        m_UFO->setDestroyed(true);
        m_UFO->setActive(false);
        m_UFO->triggerExplosion();
        
        // Award bonus points for automatic UFO destruction
        m_Score += 500;
    }
    
    // Clear all lingering entity states
    resetAllEntityStates();
    
    // Update status display to show new score
    pStatus->setScore(m_Score);
}

void GameController::resetAllEntityStates() {
    // Reset player shots - deactivate any active bullets
    for (int shot = 0; shot < pThePlayer->getNumberOfShots(); shot++) {
        if (pThePlayer->getShotActive(shot)) {
            pThePlayer->setShotActive(shot, false);
        }
    }
    
    // Clear player vapor trail
    pThePlayer->clearVaporTrail();
    
    // Reset LeadEnemy states
    if (pLeader) {
        // Deactivate LeadEnemy shot if active
        if (pLeader->getShotActive()) {
            pLeader->shotHitTarget(); // This deactivates the shot
        }
    }
    
    // Reset Fighter states  
    if (pFighter) {
        // Deactivate Fighter shot if active
        if (pFighter->getShotActive()) {
            pFighter->shotHitTarget(); // This deactivates the shot
        }
        
        // Clear Fighter mines
        for (int mine = 0; mine < pFighter->getMineCount(); mine++) {
            if (pFighter->getMineActive(mine)) {
                pFighter->mineHit(mine); // This deactivates the mine
            }
        }
    }
    
    // Reset FollowEnemy states
    if (pFollower) {
        // Clear FollowEnemy mines
        for (int mine = 0; mine < pFollower->getMineCount(); mine++) {
            if (pFollower->getMineActive(mine)) {
                pFollower->mineHit(mine); // This deactivates the mine
            }
        }
    }
    
    // Clear all enemy vapor trails (includes all enemy ships)
    pTheEnemyController->clearAllVaporTrails();
    
    // Force reset UFO explosion states immediately (don't wait for natural fade)
    if (m_UFO) {
        m_UFO->forceResetExplosion();
    }
    
    // Reset all rock dust explosions immediately
    for (size_t i = 0; i < m_Rocks.size(); i++) {
        if (m_Rocks[i]) {
            m_Rocks[i]->forceResetDustExplosion();
        }
    }
    
    // Reset the warp grid background to clear distortion effects
    pTheBorders->resetGridBackground();
}

} // namespace omegarace
