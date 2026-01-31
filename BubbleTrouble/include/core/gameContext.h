#pragma once
#include "types.h"
#include "constants.h"
#include <windows.h>


struct GameContext{
    // ===== GAME OBJEKTI =====
     Projectile harpoon;
     Hero hero;
     StaticObject leftWall, rightWall, floorWall, animatedWall;
     StaticObject backgroundInfo;
     StaticObject levelPlaceholderInfo;
     StaticObject heartInfo, heartBgInfo, heartBorderInfo;
     Balloon balloons[MAX_BALLOONS];
     HeartAnim hearts[5];
     Torch torchInfo;
     AnimatedWalls animatedWalls;

    // UI
     MenuButton menuButtons[NUM_MENU_BUTTONS];
     Button restartButtonInfo;
     Button homeButtonInfo;
     Button nextButtonInfo;

    // STATE
    InputState inputState;
    GameState gameState;
    TransitionState transitionState;

};

extern GameContext gGame;


