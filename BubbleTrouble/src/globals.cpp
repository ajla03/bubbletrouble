#include <globals.h>

const float HERO_INVINCIBLE_TIME = 0.5f;

Hero hero;
Projectile harpoon;
StaticObject leftWall, rightWall, floorWall;
StaticObject backgroundInfo;
StaticObject levelPlaceholderInfo;
StaticObject heartInfo, heartBgInfo, heartBorderInfo;
Torch torchInfo;
InputState inputState = {false};
GameState gameState = {0, maxTime, false, false, GAME_MODE_MENU, MAX_LIVES};
Balloon balloons[MAX_BALLOONS];
HeartAnim hearts[5];

MenuButton menuButtons[NUM_MENU_BUTTONS];
Button restartButtonInfo;
Button homeButtonInfo;
Button nextButtonInfo;

double timeLeft = 2000.0;
bool wasSpacePressed = false;
HFONT hFont = nullptr;
HANDLE hFontRes = nullptr;

// bitmape
HBITMAP character = NULL;
HBITMAP characterMask = NULL;
HBITMAP arrow = NULL;
HBITMAP arrowMask = NULL;
HBITMAP wall = NULL;
HBITMAP background = NULL;
HBITMAP torch = NULL;
HBITMAP torchMask = NULL;
HBITMAP levelPlaceholderBlack = NULL;
HBITMAP levelPlaceholderWhite = NULL;
HBITMAP heart = NULL;
HBITMAP heartMask = NULL;
HBITMAP heartBorder = NULL;
HBITMAP heartBorderMask = NULL;
HBITMAP heartBkg = NULL;
HBITMAP heartBkgMask = NULL;

HBITMAP restartButton = NULL;
HBITMAP restartButtonMask = NULL;
HBITMAP homeButton = NULL;
HBITMAP homeButtonMask = NULL;
HBITMAP nextButton = NULL;
HBITMAP nextButtonMask = NULL;
HBITMAP menuScreen = NULL;
HBITMAP menuCharacter = NULL;
HBITMAP menuCharacterMask = NULL;
HBITMAP hButtonsHolder = NULL;
HBITMAP hButtonsHolderMask = NULL;
