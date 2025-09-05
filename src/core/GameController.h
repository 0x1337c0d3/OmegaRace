#pragma once

#include "AudioEngine.h"
#include "Borders.h"
#include "EnemyController.h"
#include "PauseMenu.h"
#include "Player.h"
#include "Rock.h"
#include "StatusDisplay.h"
#include "UFO.h"
#include "Window.h"
#include "vmath.h"
#include <random>
#include <vector>

// Simple UFO wrapper for easier integration
namespace omegarace {

class GameController : Common {
  public:
    GameController();
    ~GameController();

    void initialize();
    void draw();
    void update(double frame);
    void handleInput(); // Replaced SDL event handling with direct input handling
    void onPause(bool paused);
    void onScreenSizeChanged(); // NEW: Handle screen size changes for fullscreen
    
    // NEW: Get player position for grid distortion effects
    Vector2f getPlayerPosition() const;
    bool isPlayerActive() const; // NEW: Check if player is active for grid distortion
    bool isWarpActive() const;   // NEW: Check if warp transition is active

  private:
    void newGame();
    void checkCollisions();
    void spawnNewWave(int ships);
    bool playerHit();
    void triggerWarpTransition(float duration = 2.0f); // NEW: Warp effect trigger
    void completeWaveCleanup();                        // NEW: Destroy all remaining rocks and UFOs when wave ends
    void resetAllEntityStates();                       // NEW: Reset all entity states to prevent carryover

    // Rock system methods
    void spawnRocks(int waveNumber);
    void updateRocks(double frame);
    void drawRocks();
    bool doesPlayerShootRock(int shot);
    bool doesRockCollideWithPlayer(int rock);

    // UFO system methods
    void spawnUFO(int waveNumber);
    void updateUFO(double frame);
    void drawUFO();
    bool doesPlayerShootUFO(int shot);
    bool doesUFOCollideWithPlayer();

    bool doesPlayerShotEnemy(int shot);
    bool doesPlayerShootLeadEnemy(int shot);
    bool doesPlayerShootFollowEnemy(int shot);
    bool doesPlayerShootFighter(int shot);
    bool doesPlayerShootFollowMine(int shot);
    bool doesPlayerShootFighterMine(int shot);
    bool doesEnemyCollideWithPlayer(int ship);
    bool doesLeadCollideWithPlayer();
    bool doesFollowCollideWithPlayer();
    bool doesFighterCollideWithPlayer();
    bool doesFollowMineHitPalyer(int mine);
    bool doesFighterMineHitPlayer(int mine);
    bool doesLeadShootPlayer();
    bool doesFighterShootPlayer();

    Fighter* pFighter = nullptr;
    FollowEnemy* pFollower = nullptr;
    LeadEnemy* pLeader = nullptr;

    std::unique_ptr<Player> pThePlayer;
    std::unique_ptr<Borders> pTheBorders;
    std::unique_ptr<EnemyController> pTheEnemyController;
    std::unique_ptr<StatusDisplay> pStatus;
    std::unique_ptr<PauseMenu> pPauseMenu;

    // Rock system
    std::vector<omegarace::Rock*> m_Rocks;
    int m_CurrentWave;
    // Note: FMOD audio system is used instead of Mix_Chunk

    // UFO system
    UFO* m_UFO;
    float m_UFOSpawnTimer;
    float m_UFOSpawnInterval;

    bool m_EndOfWave;
    int m_Score;
    int m_PlayerShips;
    int m_State;

    float m_Timer;
    int m_TimerAmount;
    std::unique_ptr<Timer> pTimer;
    bool m_Respawn;

    // Warp transition effect
    bool m_WarpActive;
    float m_WarpDuration;
    float m_WarpStartTime;
        
    bool m_IsFirstWave;
    bool m_WaitingForWarp; // NEW: Flag to indicate we're waiting for warp to complete before spawning

    // Random number generator for Rock creation
    std::mt19937 m_RandomGenerator;

    // Bonus life system
    int m_NextBonusLifeThreshold;
    void checkBonusLife(); // NEW: Check if player has earned a bonus life
    
    // Pause system
    bool m_IsPaused;
    void handlePauseInput();
    void togglePause();
    
    // Input state tracking to prevent rapid fire
    bool m_SpaceKeyWasPressed;
    bool m_SKeyWasPressed;
    bool m_CtrlKeyWasPressed;
};

} // namespace omegarace
