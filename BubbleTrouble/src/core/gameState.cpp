#include "game.h"
#include "gameContext.h"
#include "resourceManager.h"
#include "constants.h"
#include <windows.h>

void StartGame(HWND hwnd){
    gGame.gameState.currentMode = GAME_MODE_PLAYING;
    ResetGame(hwnd);
}

void ResetGame(HWND hwnd) {
    // Reset game state
    CURRENT_LEVEL.timeLeft = maxTime;
    gGame.gameState.isGameOver = false;
    gGame.gameState.isLevelCleared = false;
    CURRENT_LEVEL.activeBalloonCount = 0;
    gGame.gameState.lives = START_LIVES;
    gGame.totalScore = 0;
    gGame.displayScore = 0;
    gGame.pauseButtonInfo.isHover = false;
    if(gGame.gameState.currentMode == GAME_MODE_MENU)
        gGame.settingsState.soundState.soundEffectsOn = true;

    if (CURRENT_LEVEL.hdcCache) DeleteDC(CURRENT_LEVEL.hdcCache);
    if (CURRENT_LEVEL.hStaticCache) DeleteObject(CURRENT_LEVEL.hStaticCache);
    CURRENT_LEVEL.hdcCache = NULL;
    CURRENT_LEVEL.hStaticCache = NULL;
    CURRENT_LEVEL.staticRedraw = true;

    gGame.currentLevel = 0;

    if (CURRENT_LEVEL.hdcCache) DeleteDC(CURRENT_LEVEL.hdcCache);
    if (CURRENT_LEVEL.hStaticCache) DeleteObject(CURRENT_LEVEL.hStaticCache);
    CURRENT_LEVEL.hdcCache = NULL;
    CURRENT_LEVEL.hStaticCache = NULL;
    CURRENT_LEVEL.staticRedraw = true;

    // Reset hero position
    RECT clientRect;
    GetClientRect(hwnd, &clientRect);
    gGame.hero.x = gGame.leftWall.width + gGame.hero.width;
    gGame.hero.y = 100;
    gGame.hero.currentRow = 2;
    gGame.hero.currentFrame = 0;

    // Reset harpoon
    gGame.harpoon.isActive = false;
    gGame.harpoon.length = 0;

    // Clear all balloons
    for (int i = 0; i < MAX_BALLOONS; i++) {
        CURRENT_LEVEL.balloons[i].active = false;
    }

    //Clear powerups
    CURRENT_LEVEL.activePowerupCount = 0;
    CURRENT_LEVEL.lastPowerupSpawn = GetTickCount();
    CURRENT_LEVEL.nextPowerupSpawnTime = POWERUP_SPAWN_MIN_TIME +
        (rand() % (POWERUP_SPAWN_MAX_TIME - POWERUP_SPAWN_MIN_TIME));

    for (int i = 0; i < MAX_POWERUPS; i++)
        CURRENT_LEVEL.powerups[i].active = false;

    if(gGame.gameState.currentMode == GAME_MODE_PLAYING ||
       gGame.gameState.currentMode == GAME_OVER)
        InitLevel(hwnd);
}

void ResetBetweenLevels(HWND hwnd) {
    // Reset time
    CURRENT_LEVEL.timeLeft = maxTime;

    // Reset states
    gGame.gameState.isGameOver = false;
    gGame.gameState.isLevelCleared = false;
    gGame.pauseButtonInfo.isHover = false;
    gGame.gameState.currentMode = GAME_MODE_PLAYING;

    CURRENT_LEVEL.activeBalloonCount = 0;
    CURRENT_LEVEL.staticRedraw = true;

    // Reset hero
    RECT clientRect;
    GetClientRect(hwnd, &clientRect);
    gGame.hero.x = gGame.leftWall.width + gGame.hero.width;
    gGame.hero.y = 100;
    gGame.hero.currentRow = 2;
    gGame.hero.currentFrame = 0;

    // Reset harpoon
    gGame.harpoon.isActive = false;
    gGame.harpoon.length = 0;

    // Clear all balloons
    for (int i = 0; i < MAX_BALLOONS; i++) {
        CURRENT_LEVEL.balloons[i].active = false;
    }

    //Clear powerups
    CURRENT_LEVEL.activePowerupCount = 0;
    CURRENT_LEVEL.lastPowerupSpawn = GetTickCount();
    CURRENT_LEVEL.nextPowerupSpawnTime = POWERUP_SPAWN_MIN_TIME +
        (rand() % (POWERUP_SPAWN_MAX_TIME - POWERUP_SPAWN_MIN_TIME));

    for (int i = 0; i < MAX_POWERUPS; i++)
        CURRENT_LEVEL.powerups[i].active = false;
}

