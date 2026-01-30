#pragma once
#include "types.h"
#include "constants.h"
#include <windows.h>


// ===== GAME OBJEKTI =====
extern Projectile harpoon;
extern Hero hero;
extern StaticObject leftWall, rightWall, floorWall, animatedWall;
extern StaticObject backgroundInfo;
extern StaticObject levelPlaceholderInfo;
extern StaticObject heartInfo, heartBgInfo, heartBorderInfo;
extern Torch torchInfo;
extern InputState inputState;
extern GameState gameState;
extern Balloon balloons[MAX_BALLOONS];
extern HeartAnim hearts[5];


// ===== UI =====
extern MenuButton menuButtons[NUM_MENU_BUTTONS];
extern Button restartButtonInfo;
extern Button homeButtonInfo;
extern Button nextButtonInfo;

// ===== SYSTEM =====
extern bool wasSpacePressed;
extern const float HERO_INVINCIBLE_TIME;


extern TransitionState transitionState;
extern AnimatedWalls animatedWalls;
