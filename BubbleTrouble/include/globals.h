#pragma once
#include "types.h"
#include "constants.h"
#include <windows.h>

// ===== BITMAPE =====
extern HBITMAP character, characterMask;
extern HBITMAP arrow, arrowMask;
extern HBITMAP wall;
extern HBITMAP background;
extern HBITMAP torch, torchMask;
extern HBITMAP levelPlaceholderBlack;
extern HBITMAP levelPlaceholderWhite;
extern HBITMAP heart, heartMask;
extern HBITMAP heartBkg, heartBkgMask;
extern HBITMAP heartBorder, heartBorderMask;

extern HBITMAP menuScreen;
extern HBITMAP menuCharacter;
extern HBITMAP menuCharacterMask;
extern HBITMAP hButtonsHolder;
extern HBITMAP hButtonsHolderMask;
extern HBITMAP restartButton;
extern HBITMAP restartButtonMask;
extern HBITMAP homeButton;
extern HBITMAP homeButtonMask;
extern HBITMAP nextButton;
extern HBITMAP nextButtonMask;
extern HCURSOR gameCursor;
extern HBITMAP wall1;
extern HBITMAP wall2;
extern HBITMAP hMenuButton;
extern HBITMAP hMenuButtonMask;

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
extern HFONT hFont;
extern HANDLE hFontRes;
extern const float HERO_INVINCIBLE_TIME;


extern TransitionState transitionState;
extern AnimatedWalls animatedWalls;
