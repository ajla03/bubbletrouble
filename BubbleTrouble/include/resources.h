#pragma once

#include <windows.h>

#define IDB_HERO            101
#define IDB_HERO_MASK       102
#define IDB_ARROW           103
#define IDB_ARROW_MASK      104
#define IDB_WALL            105
#define IDB_BACKGROUND      106


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

void LoadBitmaps(HWND hwnd, HINSTANCE hInstance);
void CheckInputs(HWND hwnd);
void Update(HWND hwnd);
void RefreshScreen(HWND hwnd);
void RefreshSound();

