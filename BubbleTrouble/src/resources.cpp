#include "resources.h"

// Bitmap definicije
HBITMAP character = NULL;
HBITMAP characterMask = NULL;
HBITMAP arrow = NULL;
HBITMAP arrowMask = NULL;
HBITMAP wall = NULL;
HBITMAP background = NULL;

// Game objects
Hero hero;
Projectile harpoon;
StaticObject leftWall;
StaticObject rightWall;
StaticObject floorWall;
StaticObject backgroundInfo;
InputState inputState = {false};

Balloon balloons[MAX_BALLOONS] = {0};
int activeBalloonCount = 0;

void LoadBitmaps(HWND hwnd, HINSTANCE hInstance){
    // Učitaj bitmap-e
    character = LoadBitmap(hInstance, MAKEINTRESOURCE(IDB_HERO));
    characterMask = LoadBitmap(hInstance, MAKEINTRESOURCE(IDB_HERO_MASK));
    arrow = LoadBitmap(hInstance, MAKEINTRESOURCE(IDB_ARROW));
    arrowMask = LoadBitmap(hInstance, MAKEINTRESOURCE(IDB_ARROW_MASK));
    wall = LoadBitmap(hInstance, MAKEINTRESOURCE(IDB_WALL));
    background = LoadBitmap(hInstance, MAKEINTRESOURCE(IDB_BACKGROUND));

    if (!character || !characterMask || !arrow || !arrowMask || !wall || !background) {
        MessageBox(NULL, ("Ne mogu da učitam resurse!"), ("Greška"), MB_ICONERROR);
        return;
    }

    BITMAP bm;

    // === HARPUN SETUP ===
    if (arrow) {
        GetObject(arrow, sizeof(BITMAP), &bm);
        harpoon.width = bm.bmWidth;
        harpoon.height = bm.bmHeight;
        harpoon.dy = 10;
        harpoon.isActive = false;
        harpoon.length = 0;
    }

    // === BACKGROUND SETUP ===
    if (background) {
        GetObject(background, sizeof(BITMAP), &bm);
        backgroundInfo.width = bm.bmWidth;
        backgroundInfo.height = bm.bmHeight;
        backgroundInfo.x = 0;
        backgroundInfo.y = 0;
    }

    // === WALL SETUP ===
    if (wall) {
        GetObject(wall, sizeof(BITMAP), &bm);

        leftWall.width = bm.bmWidth / 7;
        leftWall.height = bm.bmHeight;

        rightWall.width = bm.bmWidth / 7;
        rightWall.height = bm.bmHeight;

        floorWall.width = bm.bmWidth;
        floorWall.height = bm.bmHeight / 3;
    }

    // === HERO SETUP ===
    if (character) {
        GetObject(character, sizeof(BITMAP), &bm);
        hero.width = bm.bmWidth / 4;
        hero.height = bm.bmHeight / 3;
        hero.x = leftWall.width + hero.width;
        hero.y = 100;
        hero.dx = 3;
        hero.dy = 3;
        hero.currentRow = 2;
        hero.currentFrame = 0;
        hero.animCounter = 0;
    }

    // === INPUT STATE SETUP ===
    inputState.wasSpacePressed = false;

    InitBalloon(0, 200, 100, 20, 2.5f, RGB(255, 0, 0));    // Crvena
    InitBalloon(1, 400, 150, 40, -2.0f, RGB(0, 255, 0));   // Zelena
    InitBalloon(2, 600, 120, 80, 3.0f, RGB(0, 100, 255));  // Plava
}

void InitBalloon(int index, float x, float y, float radius, float speedX, COLORREF color) {
    if (index >= MAX_BALLOONS) return;

    balloons[index].x = x;
    balloons[index].y = y;
    balloons[index].radius = radius;
    balloons[index].speedX = speedX;
    balloons[index].speedY = 0;
    balloons[index].active = true;
    balloons[index].color = color;
    activeBalloonCount++;
}


