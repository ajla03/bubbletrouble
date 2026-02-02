#include "resources.h"
#include "gameContext.h"
#include "resourceManager.h"
#include "game.h"


void LoadBitmaps(HWND hwnd, HINSTANCE hInstance){
    RECT clientRect;
    GetClientRect(hwnd, &clientRect);

    gRes.LoadAll(hInstance);

    BITMAP bm;

    // SRCA //
    GetObject(gRes.heart, sizeof(BITMAP), &bm);
    gGame.heartInfo.width = bm.bmWidth / 10;
    gGame.heartInfo.height = bm.bmHeight;
    gGame.heartInfo.x = 0;
    gGame.heartInfo.y = 0;

    GetObject(gRes.heartBkg, sizeof(BITMAP), &bm);
    gGame.heartBgInfo.width = bm.bmWidth;
    gGame.heartBgInfo.height = bm.bmHeight;
    gGame.heartBgInfo.x = 0;
    gGame.heartBgInfo.y = 0;

    GetObject(gRes.heartBorder, sizeof(BITMAP), &bm);
    gGame.heartBorderInfo.width = bm.bmWidth;
    gGame.heartBorderInfo.height = bm.bmHeight;
    gGame.heartBorderInfo.x = 0;
    gGame.heartBorderInfo.y = 0;

    for (int i = 0; i < 5; i++) {
    gGame.hearts[i].currentFrame = 0;
    gGame.hearts[i].animCounter = 8;
    }

    // === HARPUN SETUP ===
    if (gRes.arrow) {
        GetObject(gRes.arrow, sizeof(BITMAP), &bm);
        gGame.harpoon.width = bm.bmWidth;
        gGame.harpoon.height = bm.bmHeight;
        gGame.harpoon.dy = 10;
        gGame.harpoon.isActive = false;
        gGame.harpoon.length = 0;
    }

    // === BACKGROUND SETUP ===
    if (gRes.background) {
        GetObject(gRes.background, sizeof(BITMAP), &bm);
        CURRENT_LEVEL.backgroundInfo.width = bm.bmWidth;
        CURRENT_LEVEL.backgroundInfo.height = bm.bmHeight;
        CURRENT_LEVEL.backgroundInfo.x = 0;
        CURRENT_LEVEL.backgroundInfo.y = 0;
    }

    if(gRes.torch){
        GetObject(gRes.torch, sizeof(BITMAP), &bm);
        gGame.torchInfo.width = bm.bmWidth / 4;
        gGame.torchInfo.height = bm.bmHeight /2 ;
        gGame.torchInfo.x = 0;
        gGame.torchInfo.y = 0;
        gGame.torchInfo.currentFrame = gGame.torchInfo.currentRow = 0;
    }

    // === WALL SETUP ===
    if (gRes.wall) {
        GetObject(gRes.wall, sizeof(BITMAP), &bm);

        gGame.leftWall.width = bm.bmWidth/13;
        gGame.leftWall.height = bm.bmHeight;

        gGame.rightWall.width = bm.bmWidth/13;
        gGame.rightWall.height = bm.bmHeight;

        gGame.floorWall.width = bm.bmWidth;
        gGame.floorWall.height = bm.bmHeight/6;
    }


    // ==== LEVEL PLACEHOLDER SETUP ==== //
    if(gRes.levelPlaceholderWhite){
        GetObject(gRes.levelPlaceholderWhite, sizeof(BITMAP), &bm);

        gGame.levelPlaceholderInfo.height = bm.bmHeight;
        gGame.levelPlaceholderInfo.width = bm.bmWidth;
        gGame.levelPlaceholderInfo.x = 0;
        gGame.levelPlaceholderInfo.y = 0;
    }

    // === HERO SETUP ===
    if (gRes.character) {
        GetObject(gRes.character, sizeof(BITMAP), &bm);
        gGame.hero.width = bm.bmWidth / 4;
        gGame.hero.height = bm.bmHeight / 3;
        gGame.hero.x = gGame.leftWall.width + gGame.hero.width;
        gGame.hero.y = 100;
        gGame.hero.dx = 5;
        gGame.hero.dy = 3;
        gGame.hero.currentRow = 2;
        gGame.hero.currentFrame = 0;
        gGame.hero.animCounter = 0;
    }

    if(gRes.homeButton){
        GetObject(gRes.homeButton, sizeof(BITMAP), &bm);
        gGame.homeButtonInfo.width = bm.bmWidth;
        gGame.homeButtonInfo.height = bm.bmHeight;
        gGame.homeButtonInfo.isHover = false;
    }

    if(gRes.restartButton){
        GetObject(gRes.restartButton, sizeof(BITMAP), &bm);
        gGame.restartButtonInfo.width = bm.bmWidth;
        gGame.restartButtonInfo.height = bm.bmHeight;
        gGame.restartButtonInfo.isHover = false;
    }

    if(gRes.nextButton){
        GetObject(gRes.nextButton, sizeof(BITMAP), &bm);
        gGame.nextButtonInfo.width = bm.bmWidth;
        gGame.nextButtonInfo.height = bm.bmHeight;
        gGame.nextButtonInfo.isHover = false;
    }

    if(gRes.wall1){
        GetObject(gRes.wall1, sizeof(BITMAP), &bm);
        gGame.animatedWall.height = bm.bmHeight;
        gGame.animatedWall.width = bm.bmWidth;
    }

    // === INPUT STATE SETUP ===
    gGame.inputState.wasSpacePressed = false;
    InitializeMenu(hwnd);

    //InitBalloon(0, 200, 100, 20, 2.5f, RGB(255, 0, 0));    // Crvena
    InitBalloon(1, 400, 150, 40, -2.0f, RGB(0, 255, 0));   // Zelena
    //InitBalloon(2, 600, 120, 80, 3.0f, RGB(0, 100, 255));  // Plava
}


float GetBounceSpeedForRadius(float radius){
  float t = radius / MAX_RADIUS;
  return MIN_BOUNCE_SPEED + t * (MAX_BOUNCE_SPEED - MIN_BOUNCE_SPEED); //veci balon veci skok manji balon manji skok
}


void InitBalloon(int index, float x, float y, float radius, float speedX, COLORREF color) {
    if (index >= MAX_BALLOONS) return;

    CURRENT_LEVEL.balloons[index].x = x;
    CURRENT_LEVEL.balloons[index].y = y;
    CURRENT_LEVEL.balloons[index].radius = radius;
    CURRENT_LEVEL.balloons[index].speedX = speedX * 0.85f;
    CURRENT_LEVEL.balloons[index].speedY = 0.0f;
    CURRENT_LEVEL.balloons[index].bounceSpeed = GetBounceSpeedForRadius(radius);
    CURRENT_LEVEL.balloons[index].active = true;
    CURRENT_LEVEL.balloons[index].color = color;
    CURRENT_LEVEL.activeBalloonCount++;
}


