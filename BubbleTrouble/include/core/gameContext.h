#pragma once
#include "types.h"
#include "constants.h"
#include <windows.h>

struct Level{
     StaticObject backgroundInfo;
     Pillar longWall;
     Door door;
     Balloon balloons[MAX_BALLOONS];
     int activeBalloonCount;
     double timeLeft;
     int levelScore;
     Pillar pillar1;
     Pillar pillar2;
     HBITMAP hStaticCache = NULL;
     HDC hdcCache = NULL;
     bool staticRedraw = true;
};

struct GameContext{

    // ======== LEVELS ====== //
     Level levels[MAX_LEVELS];
     int currentLevel;
     int totalScore;
     int displayScore;

    // ===== SHARED GAME OBJEKTI =====
     Projectile harpoon;
     Hero hero;

     StaticObject leftWall, rightWall, floorWall, animatedWall;
     StaticObject levelPlaceholderInfo;
     StaticObject playerHolderInfo, scoreHolderInfo;
     StaticObject heartInfo, heartBgInfo, heartBorderInfo;

     HeartAnim hearts[5];
     Torch torchInfo;
     AnimatedWalls animatedWalls;
     StaticObject logoInfo;


    // UI
     MenuButton menuButtons[NUM_MENU_BUTTONS];
     Button restartButtonInfo;
     Button homeButtonInfo;
     Button nextButtonInfo;
     Button pauseButtonInfo;
     Button unpauseButtonInfo;

    // STATE
    InputState inputState;
    GameState gameState;
    TransitionState transitionState;

};

extern GameContext gGame;
