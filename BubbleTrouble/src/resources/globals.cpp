#include <globals.h>

const float HERO_INVINCIBLE_TIME = 0.5f;

Hero hero;
Projectile harpoon;
StaticObject leftWall, rightWall, floorWall;
StaticObject backgroundInfo;
StaticObject levelPlaceholderInfo;
StaticObject heartInfo, heartBgInfo, heartBorderInfo, animatedWall;
Torch torchInfo;
InputState inputState = {false};
GameState gameState = {0, maxTime, false, false, GAME_MODE_MENU, MAX_LIVES, false, false};
Balloon balloons[MAX_BALLOONS];
HeartAnim hearts[5];

MenuButton menuButtons[NUM_MENU_BUTTONS];
Button restartButtonInfo;
Button homeButtonInfo;
Button nextButtonInfo;

double timeLeft = 2000.0;
bool wasSpacePressed = false;

TransitionState transitionState = TRANSITION_NONE;
AnimatedWalls animatedWalls = {0,0,12.0f};
