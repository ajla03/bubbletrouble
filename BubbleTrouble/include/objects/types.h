#pragma once
#include<windows.h>

// ======== POMOCNE STRUKTURE ======= //
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



struct Door : public StaticObject{
 int originalWidth, originalHeight;
 bool active = true;
};

struct Pillar : public StaticObject{
 int originalWidth, originalHeight;
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
    GAME_OVER,
};

struct MenuButton {
    RECT rect;
    const char* text;
    bool isHovered;
};


struct GameState{
    bool isGameOver;
    bool isLevelCleared;
    GameMode currentMode;
    int lives;
    bool pendingHome;
    bool pendingRestart;
    bool pendingNextLevel;
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

// used for MENU, RESTART and NEXT button
struct Button {
   int x, y;
   int width, height;
   bool isHover;
};


enum TransitionState{
 TRANSITION_NONE,
 TRANSITION_CLOSING,
 TRANSITION_OPENING,
 TRANSITION_WAIT,
 };

struct AnimatedWalls{
 int wallTopY ;
 int wallBottomY;
 float wallSpeed;
 DWORD transitionWaitStart = 0;
};


