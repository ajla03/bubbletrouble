#pragma once

#include <windows.h>

#define IDB_HERO            101
#define IDB_HERO_MASK       102
#define IDB_ARROW           103
#define IDB_ARROW_MASK      104
#define IDB_WALL            105
#define IDB_BACKGROUND      106

#define MAX_BALLOONS 20
#define GRAVITY 0.2f
#define BOUNCE_DAMPING 0.9f
#define MIN_RADIUS 10.0f
#define MIN_BOUNCE_SPEED 3.0f

struct Hero{
 int x, y;
 int dx, dy;
 int width, height;
 int currentFrame, currentRow;
 int animCounter;
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
    bool active;
    COLORREF color;
};

struct InputState{
 bool wasSpacePressed;
};

extern HBITMAP character, characterMask;
extern HBITMAP arrow, arrowMask;
extern HBITMAP wall;
extern HBITMAP background;

extern bool wasSpacePressed;
extern bool isHarpoonActive;

extern Projectile harpoon;
extern Hero hero;
extern StaticObject leftWall;
extern StaticObject rightWall;
extern StaticObject floorWall;
extern StaticObject backgroundInfo;
extern InputState inputState;

extern Balloon balloons[MAX_BALLOONS];
extern int activeBalloonCount;

extern double maxTime;
extern double timeLeft;
extern bool isGameOver;

void LoadBitmaps(HWND hwnd, HINSTANCE hInstance);
void CheckInputs(HWND hwnd);
void Update(HWND hwnd);
void RefreshScreen(HWND hwnd);
void RefreshSound();

void InitBalloon(int index, float x, float y, float radius, float speedX, COLORREF color);
void UpdateBalloons(HWND hwnd);
void CheckCollisions();
void SplitBalloon(int index);
void DrawBalloonGDI(HDC hdc, Balloon* b);

