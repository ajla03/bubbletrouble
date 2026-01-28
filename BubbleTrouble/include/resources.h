#pragma once

#include <windows.h>

#define IDB_HERO            101
#define IDB_HERO_MASK       102
#define IDB_ARROW           103
#define IDB_ARROW_MASK      104
#define IDB_WALL            105
#define IDB_BACKGROUND      106
#define IDB_TORCH           107
#define IDB_TORCH_MASK      108
#define IDB_LEVEL_WHITE     109
#define IDB_LEVEL_BLACK     110
// HEARTS
#define IDB_HEART           111
#define IDB_HEART_MASK      112
#define IDB_HEART_BORDER    113
#define IDB_HEART_BORDER_MASK 114
#define IDB_HEART_BKG       115
#define IDB_HEART_BKG_MASK  116
#define IDB_MENU_SCREEN     117
#define IDB_MENU_CHARACTER      118
#define IDB_MENU_CHARACTER_MASK 119
#define IDB_BUTTONS_HOLDER 120
#define IDB_BUTTONS_HOLDER_MASK 121
#define IDB_HOME            122
#define IDB_HOME_MASK       123
#define IDB_RESTART         124
#define IDB_RESTART_MASK    125

#define HEART_FRAMES        10
#define HEART_ANIM_SPEED     5
#define MAX_LIVES            3

#define IDR_HARPOON_SOUND   201
#define IDR_BALLOON_POP     202
#define IDR_MINI_FONT       301

#define MAX_BALLOONS 20
#define BOUNCE_DAMPING 0.9f
#define MIN_RADIUS 10.0f
#define BALLOON_GRAVITY  0.7f
#define MAX_FALL_SPEED   8.0f
#define MIN_BOUNCE_SPEED   10.0f
#define MAX_BOUNCE_SPEED   13.0f
#define SPLIT_BOOST_FACTOR 1.2f
#define MAX_RADIUS 32.0f


#define maxTime 2000.0

struct Hero{
 int x, y;
 int dx, dy;
 int width, height;
 int currentFrame, currentRow;
 int animCounter;
 float heroHitCooldown; // in secs
};

struct Projectile{
 int x, y;
 int width, height;
 int dy;
 int length;
 bool isActive;
};

struct StaticObject{
 int x, y;
 int width, height;
};

struct Balloon {
    float x, y;
    float speedX, speedY;
    float radius;
    float bounceSpeed;
    bool active;
    COLORREF color;
};

struct InputState{
 bool wasSpacePressed;
};

enum GameMode {
    GAME_MODE_MENU,
    GAME_MODE_PLAYING,
    GAME_OVER
};

struct MenuButton {
    RECT rect;
    const char* text;
    bool isHovered;
};


struct GameState{
    int activeBalloonCount;
    double timeLeft;
    bool isGameOver;
    bool isLevelCleared;
    GameMode currentMode;
    int lives;
};

struct HeartAnim{
    int currentFrame;
    int animCounter;
};

struct Torch{
 int x, y;
 int width, height;
 int currentFrame, currentRow;
 int animCounter = 0;
};

extern HBITMAP character, characterMask;
extern HBITMAP arrow, arrowMask;
extern HBITMAP wall;
extern HBITMAP background;
extern HBITMAP torch, torchMask;
extern HBITMAP levelPlaceholderBlack;
extern HBITMAP levelPlaceholderWhite;
extern HBITMAP heartMask, heart;
extern HBITMAP heartBkgMask, heartBkg;
extern HBITMAP heartBorderMask, heartBorder;

extern HBITMAP menuScreen;
extern HBITMAP menuCharacter;
extern HBITMAP menuCharacterMask;
extern HBITMAP hButtonsHolder;
extern HBITMAP hButtonsHolderMask;

extern HBITMAP restartButton;
extern HBITMAP restartButtonMask;
extern HBITMAP homeButton;
extern HBITMAP homeButtonMask;

#define NUM_MENU_BUTTONS 3
extern MenuButton menuButtons[NUM_MENU_BUTTONS];

extern bool wasSpacePressed;
extern bool isHarpoonActive;
extern HFONT hFont;
extern HANDLE hFontRes;

extern Projectile harpoon;
extern Hero hero;
extern StaticObject leftWall;
extern StaticObject rightWall;
extern StaticObject floorWall;
extern StaticObject backgroundInfo;
extern Torch torchInfo;
extern InputState inputState;
extern GameState gameState;
extern StaticObject levelPlaceholderInfo;
extern StaticObject heartInfo, heartBgInfo, heartBorderInfo;
extern StaticObject restartButtonInfo, homeButtonInfo;
extern const float HERO_INVINCIBLE_TIME;

extern Balloon balloons[MAX_BALLOONS];
extern HeartAnim hearts[5];

void LoadCustomFont();
void LoadBitmaps(HWND hwnd, HINSTANCE hInstance);
void CheckInputs(HWND hwnd);
void Update(HWND hwnd);
void RefreshScreen(HWND hwnd);
void RefreshSound();

float GetBounceSpeedForRadius(float radius);
void InitBalloon(int index, float x, float y, float radius, float speedX, COLORREF color);
void UpdateBalloons(HWND hwnd);
void CheckCollisions();
void SplitBalloon(int index);
void DrawBalloonGDI(HDC hdc, Balloon* b);

void InitializeMenu(HWND hwnd);
void RenderMenu(HWND hwnd);
void HandleMenuClick(HWND hwnd, int x, int y);
void HandleMenuMouseMove(HWND hwnd, int x, int y);
void StartGame(HWND hwnd);
void ResetGame(HWND hwnd);

void DrawGameOverScreen(HDC hdc, RECT rect);
