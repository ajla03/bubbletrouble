#include "game.h"
#include "gameContext.h"
#include "resourceManager.h"
#include "constants.h"
#include <windows.h>

static void ResetLevelsGraphicsCache();

void StartGame(HWND hwnd){
    gGame.gameState.currentMode = GAME_MODE_PLAYING;
    ResetGame(hwnd);
}

void ResetGame(HWND hwnd) {
    RECT rect;
    GetClientRect(hwnd, &rect);

    gGame.currentLevel = 0;

    ResetLevelsGraphicsCache();

    CURRENT_LEVEL.timeLeft = maxTime;
    gGame.gameState.isGameOver = false;
    gGame.gameState.isLevelCleared = false;
    CURRENT_LEVEL.activeBalloonCount = 0;
    gGame.totalScore = 0;
    gGame.displayScore = 0;
    gGame.pauseButtonInfo.isHover = false;
    gGame.gameState.balloonsAreFrozen = false;
    gGame.gameState.freezeTimeLeft = 0;

    // Reset Player 1
    gGame.player1Stats.lives = START_LIVES;
    gGame.player1Stats.score = 0;
    gGame.player1Stats.displayScore = 0;
    gGame.player1Stats.isAlive = true;
    for(int i = 0; i < 5; i++) {
        gGame.player1Stats.hearts[i].currentFrame = 0;
        gGame.player1Stats.hearts[i].animCounter = 0;
    }

   if(gGame.gameState.isMultiplayer){
        // Reset Player 2
        gGame.player2Stats.lives = START_LIVES;
        gGame.player2Stats.score = 0;
        gGame.player2Stats.displayScore = 0;
        gGame.player2Stats.isAlive = true;
        gGame.hero2.x = rect.right - gGame.rightWall.width - gGame.hero2.width - 50;
        gGame.hero2.y = rect.bottom - gGame.floorWall.height - gGame.hero2.height;

        // --- DODATI OVO ---
        gGame.hero2.floorY = 0;
        gGame.hero2.isOnLadder = false;
        // ------------------

        for(int i = 0; i < 5; i++) {
            gGame.player2Stats.hearts[i].currentFrame = 0;
            gGame.player2Stats.hearts[i].animCounter = 0;
        }
    }

    // Reset hero position
    RECT clientRect;
    GetClientRect(hwnd, &clientRect);
    gGame.hero.x = gGame.leftWall.width + gGame.hero.width;
    gGame.hero.y = 100;
    gGame.hero.currentRow = 2;
    gGame.hero.currentFrame = 0;
    gGame.hero.isOnLadder = false;
    gGame.hero.floorY = 0;  // InitLevel ce postaviti tacnu vrijednost

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

    gGame.hero.heroHitCooldown=0;
    gGame.hero2.heroHitCooldown=0;

    if(gGame.gameState.currentMode == GAME_MODE_PLAYING ||
       gGame.gameState.currentMode == GAME_OVER){
        ResetScoreFlag();
        InitLevel(hwnd);
    }
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

    gGame.gameState.balloonsAreFrozen = false;
    gGame.gameState.freezeTimeLeft = 0;

    // Reset hero
    RECT clientRect;
    GetClientRect(hwnd, &clientRect);
    gGame.hero.x = gGame.leftWall.width + gGame.hero.width;
    gGame.hero.y = 100;
    gGame.hero.currentRow = 2;
    gGame.hero.currentFrame = 0;
    gGame.hero.isOnLadder = false;
    gGame.hero.floorY = 0;  // InitLevel ce postaviti tacnu vrijednost

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

    if(gGame.gameState.isMultiplayer) {
        RECT clientRect;
        GetClientRect(hwnd, &clientRect);

        gGame.hero2.x = clientRect.right - gGame.rightWall.width - gGame.hero2.width - 50;
        gGame.hero2.y = 100;
        gGame.hero2.currentRow = 2;
        gGame.hero2.currentFrame = 0;

        gGame.hero2.floorY = 0;
        gGame.hero2.isOnLadder = false;

        gGame.harpoon2.isActive = false;
        gGame.harpoon2.length = 0;
    }
}


void ResetLevelsGraphicsCache() {
    if (CURRENT_LEVEL.hdcCache) {
        DeleteDC(CURRENT_LEVEL.hdcCache);
        CURRENT_LEVEL.hdcCache = NULL;
    }

    if (CURRENT_LEVEL.hStaticCache) {
        DeleteObject(CURRENT_LEVEL.hStaticCache);
        CURRENT_LEVEL.hStaticCache = NULL;
    }

    CURRENT_LEVEL.staticRedraw = true;
}


void ResetScoreFlag(){
    gGame.gameState.isScoreSaved = false;
}
