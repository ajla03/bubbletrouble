#include "../include/resources.h"
#include "globals.h"
#include "resourceManager.h"
#include "game.h"

void LoadBitmaps(HWND hwnd, HINSTANCE hInstance){
    RECT clientRect;
    GetClientRect(hwnd, &clientRect);

    gRes.LoadAll(hInstance);

    BITMAP bm;

    // SRCA //
    GetObject(gRes.heart, sizeof(BITMAP), &bm);
    heartInfo.width = bm.bmWidth / 10;
    heartInfo.height = bm.bmHeight;
    heartInfo.x = 0;
    heartInfo.y = 0;

    GetObject(gRes.heartBkg, sizeof(BITMAP), &bm);
    heartBgInfo.width = bm.bmWidth;
    heartBgInfo.height = bm.bmHeight;
    heartBgInfo.x = 0;
    heartBgInfo.y = 0;

    GetObject(gRes.heartBorder, sizeof(BITMAP), &bm);
    heartBorderInfo.width = bm.bmWidth;
    heartBorderInfo.height = bm.bmHeight;
    heartBorderInfo.x = 0;
    heartBorderInfo.y = 0;

    for (int i = 0; i < 5; i++) {
    hearts[i].currentFrame = 0;
    hearts[i].animCounter = 8;
    }

    // === HARPUN SETUP ===
    if (gRes.arrow) {
        GetObject(gRes.arrow, sizeof(BITMAP), &bm);
        harpoon.width = bm.bmWidth;
        harpoon.height = bm.bmHeight;
        harpoon.dy = 10;
        harpoon.isActive = false;
        harpoon.length = 0;
    }

    // === BACKGROUND SETUP ===
    if (gRes.background) {
        GetObject(gRes.background, sizeof(BITMAP), &bm);
        backgroundInfo.width = bm.bmWidth;
        backgroundInfo.height = bm.bmHeight;
        backgroundInfo.x = 0;
        backgroundInfo.y = 0;
    }

    if(gRes.torch){
        GetObject(gRes.torch, sizeof(BITMAP), &bm);
        torchInfo.width = bm.bmWidth / 4;
        torchInfo.height = bm.bmHeight /2 ;
        torchInfo.x = 0;
        torchInfo.y = 0;
        torchInfo.currentFrame = torchInfo.currentRow = 0;
    }

    // === WALL SETUP ===
    if (gRes.wall) {
        GetObject(gRes.wall, sizeof(BITMAP), &bm);

        leftWall.width = bm.bmWidth /2 ;
        leftWall.height = bm.bmHeight;

        rightWall.width = bm.bmWidth / 2;
        rightWall.height = bm.bmHeight;

        floorWall.width = bm.bmWidth;
        floorWall.height = bm.bmHeight;
    }


    // ==== LEVEL PLACEHOLDER SETUP ==== //
    if(gRes.levelPlaceholderWhite){
        GetObject(gRes.levelPlaceholderWhite, sizeof(BITMAP), &bm);

        levelPlaceholderInfo.height = bm.bmHeight;
        levelPlaceholderInfo.width = bm.bmWidth;
        levelPlaceholderInfo.x = 0;
        levelPlaceholderInfo.y = 0;
    }

    // === HERO SETUP ===
    if (gRes.character) {
        GetObject(gRes.character, sizeof(BITMAP), &bm);
        hero.width = bm.bmWidth / 4;
        hero.height = bm.bmHeight / 3;
        hero.x = leftWall.width + hero.width;
        hero.y = 100;
        hero.dx = 4;
        hero.dy = 3;
        hero.currentRow = 2;
        hero.currentFrame = 0;
        hero.animCounter = 0;
    }

    if(gRes.homeButton){
        GetObject(gRes.homeButton, sizeof(BITMAP), &bm);
        homeButtonInfo.width = bm.bmWidth;
        homeButtonInfo.height = bm.bmHeight;
        homeButtonInfo.isHover = false;
    }

    if(gRes.restartButton){
        GetObject(gRes.restartButton, sizeof(BITMAP), &bm);
        restartButtonInfo.width = bm.bmWidth;
        restartButtonInfo.height = bm.bmHeight;
        restartButtonInfo.isHover = false;
    }

    if(gRes.nextButton){
        GetObject(gRes.nextButton, sizeof(BITMAP), &bm);
        nextButtonInfo.width = bm.bmWidth;
        nextButtonInfo.height = bm.bmHeight;
        nextButtonInfo.isHover = false;
    }

    if(gRes.wall1){
        GetObject(gRes.wall1, sizeof(BITMAP), &bm);
        animatedWall.height = bm.bmHeight;
        animatedWall.width = bm.bmWidth;
    }

    // === INPUT STATE SETUP ===
    inputState.wasSpacePressed = false;
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

    balloons[index].x = x;
    balloons[index].y = y;
    balloons[index].radius = radius;
    balloons[index].speedX = speedX * 0.85f;
    balloons[index].speedY = 0.0f;
    balloons[index].bounceSpeed = GetBounceSpeedForRadius(radius);
    balloons[index].active = true;
    balloons[index].color = color;
    gameState.activeBalloonCount++;
}


