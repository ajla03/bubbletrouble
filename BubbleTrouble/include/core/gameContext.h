#pragma once
#include "types.h"
#include "constants.h"
#include "loading.h"
#include <windows.h>

struct Level{
     StaticObject backgroundInfo;
     Pillar longWall;
     Door door;
     Balloon balloons[MAX_BALLOONS];
     int activeBalloonCount;
     double timeLeft = maxTime;
     int levelScore;
     Pillar pillar1;
     Pillar pillar2;
     HBITMAP hStaticCache = NULL;
     HDC hdcCache = NULL;
     bool staticRedraw = true;

     PowerUp powerups[MAX_POWERUPS];
     int activePowerupCount;
     DWORD lastPowerupSpawn;
     DWORD nextPowerupSpawnTime;
     StaticObject ladder;        // merdevine (Level 7)
     int ladderOrigW, ladderOrigH;
};


struct GameContext{

    // ======== LEVELS ====== //
     Level levels[MAX_LEVELS];
     int currentLevel;
     int totalScore;      // Combined score for level transitions
     int displayScore;    // Old - keep for compatibility

    // ===== SHARED GAME OBJEKTI =====
     Projectile harpoon;
     Projectile harpoon2;
     Hero hero;
     Hero hero2;

     StaticObject leftWall, rightWall, floorWall, animatedWall;
     StaticObject levelPlaceholderInfo;
     StaticObject playerHolderInfo, scoreHolderInfo;
     StaticObject player2HolderInfo, score2HolderInfo;
     StaticObject heartInfo, heartBgInfo, heartBorderInfo;

     HeartAnim hearts[5];  // Old - keep for compatibility
     Torch torchInfo;
     AnimatedWalls animatedWalls;
     StaticObject logoInfo;

     // === NEW: SEPARATE PLAYER STATS ===
     PlayerStats player1Stats;
     PlayerStats player2Stats;

    // UI
     MenuButton menuButtons[NUM_MENU_BUTTONS];
     Button restartButtonInfo;
     Button homeButtonInfo;
     Button nextButtonInfo;
     Button pauseButtonInfo;
     Button unpauseButtonInfo;
     Button soundButtonInfo;
     Button backButtonInfo;
     Button settingsMusicButtonInfo;
     Button settingsSoundButtonInfo;
     Button player1, player2;
     Button dashboardButtonInfo;
     RECT helpIconRect;
     Button loginButtonInfo;
     bool helpIconHovered;

    // STATE
    InputState inputState;
    GameState gameState;
    TransitionState transitionState;
    SettingsState settingsState;
    LoadingState loadingState;
    LoginInput loginInput;

    // Player Name
    char playerName[12];
    int unlockedLevel;
};

extern GameContext gGame;
