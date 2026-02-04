#include "resources.h"
#include "gameContext.h"
#include "game.h"
#include <windows.h>
#include <iostream>

void UpdateHeroCoolDown(float dt){
    if(gGame.hero.heroHitCooldown > 0)
        gGame.hero.heroHitCooldown -=dt;
}

void UpdateScoreAnimation()
{
    int diff = gGame.totalScore - gGame.displayScore;

    if (diff > 0)
    {
        int step = std::max(1, diff / 10);
        gGame.displayScore += step;
    }
}


void UpdateHearts(){
 for (int i = 0; i < gGame.gameState.lives; i++) {
    gGame.hearts[i].animCounter++;

    if (gGame.hearts[i].animCounter >= HEART_ANIM_SPEED) {
        gGame.hearts[i].animCounter = 0;
        gGame.hearts[i].currentFrame++;

        if (gGame.hearts[i].currentFrame >= HEART_FRAMES) {
            gGame.hearts[i].currentFrame = 0;
        }
    }
 }
}

void Update(HWND hwnd){
    if(gGame.gameState.pendingHome || gGame.transitionState == TRANSITION_CLOSING || gGame.transitionState == TRANSITION_WAIT )
       {
        UpdateWallTransition(hwnd);
        return;
       }
    else if(gGame.gameState.currentMode == GAME_MODE_PAUSE || gGame.gameState.currentMode == GAME_OVER ) return;

    printf("%d", gGame.gameState.currentMode);

    printf("GDI Objects: %ld\n", GetGuiResources(GetCurrentProcess(), GR_GDIOBJECTS));
     if (!gGame.gameState.isGameOver && CURRENT_LEVEL.timeLeft > 0 && !gGame.gameState.isLevelCleared) {
        CURRENT_LEVEL.timeLeft -= 1.0;
    } else if (CURRENT_LEVEL.timeLeft <= 0) {
        CURRENT_LEVEL.timeLeft = 0;
        gGame.gameState.isGameOver = true;
    }
    if (gGame.harpoon.isActive){
        RECT rect;
        GetClientRect(hwnd, &rect);
        gGame.harpoon.length += gGame.harpoon.dy;
        gGame.harpoon.y = rect.bottom - gGame.floorWall.height;

        if (gGame.harpoon.length >= rect.bottom){
            gGame.harpoon.isActive = false;
        }
    }

    // update baklji
    gGame.torchInfo.animCounter++;
    if(gGame.torchInfo.animCounter > 5){
    gGame.torchInfo.currentFrame++;
    if (gGame.torchInfo.currentFrame >= 4) {
        gGame.torchInfo.currentFrame = 0;
        gGame.torchInfo.currentRow = (gGame.torchInfo.currentRow + 1) % 2;
     }
     gGame.torchInfo.animCounter = 0;
    }


    // ==== update aktivnih vrata ====== //
    if(CURRENT_LEVEL.door.width > 0 && CURRENT_LEVEL.door.height > 0){
        float sectionLeft  = gGame.leftWall.width;
        float sectionRight = CURRENT_LEVEL.longWall.x;

        if (AreSectionBalloonsDestroyed(sectionLeft, sectionRight))
        {
            CURRENT_LEVEL.door.active = false;
        }
    }
// ==== Level 4: update stubova ====== //
    if(gGame.currentLevel == 3) {
        RECT rect;
        GetClientRect(hwnd, &rect);

        if(CURRENT_LEVEL.pillar1.width > 0) {
            bool balloon0Destroyed = true;
            for(int i = 0; i < MAX_BALLOONS; i++) {
                if(CURRENT_LEVEL.balloons[i].active &&
                   CURRENT_LEVEL.balloons[i].color == RGB(255, 200, 0)) {
                    balloon0Destroyed = false;
                    break;
                }
            }

            if(balloon0Destroyed) {
                CURRENT_LEVEL.pillar1.width = 0;  // Ukloni prvi stub
            }
        }

        if(CURRENT_LEVEL.pillar2.width > 0) {
            bool balloon1Destroyed = true;

            for(int i = 0; i < MAX_BALLOONS; i++) {
                if(CURRENT_LEVEL.balloons[i].active &&
                   CURRENT_LEVEL.balloons[i].color == RGB(255, 100, 0)) {
                    balloon1Destroyed = false;
                    break;
                }
            }

            if(balloon1Destroyed) {
                CURRENT_LEVEL.pillar2.width = 0;  // Ukloni drugi stub
            }
        }
    }
     if(gGame.gameState.hasShield && gGame.gameState.shieldTimeLeft > 0) {
        gGame.gameState.shieldTimeLeft--;
        if(gGame.gameState.shieldTimeLeft <= 0) {
            gGame.gameState.hasShield = false;
        }
    }

    if(gGame.gameState.hasDoubleShot && gGame.gameState.doubleShotTimeLeft > 0) {
        gGame.gameState.doubleShotTimeLeft--;
        if(gGame.gameState.doubleShotTimeLeft <= 0) {
            gGame.gameState.hasDoubleShot = false;
        }
    }
    UpdateHeroCoolDown(0.016f);

    UpdateBalloons(hwnd);

    UpdateHearts();

    UpdatePowerups(hwnd);

    CheckCollisions();

    UpdateScoreAnimation();

    UpdateWallTransition(hwnd);

}

void UpdateWallTransition(HWND hwnd){
    if(gGame.transitionState == TRANSITION_CLOSING){
        gGame.animatedWalls.wallTopY +=gGame.animatedWalls.wallSpeed;
        gGame.animatedWalls.wallBottomY -=gGame.animatedWalls.wallSpeed;

        if(gGame.animatedWalls.wallTopY + gGame.animatedWall.height >= gGame.animatedWalls.wallBottomY){

            if(gGame.gameState.pendingRestart){
                ResetGame(hwnd);
            }

            gGame.animatedWalls.transitionWaitStart = GetTickCount();
            gGame.transitionState = TRANSITION_WAIT;
        }
     }else if (gGame.transitionState == TRANSITION_WAIT)
    {
        if (GetTickCount() - gGame.animatedWalls.transitionWaitStart >= 400)
        {
            if(gGame.gameState.pendingHome){
                gGame.gameState.currentMode = GAME_MODE_MENU;
                gGame.gameState.isGameOver = false;
                gGame.gameState.isLevelCleared = false;
                ResetGame(hwnd);
            }else if(gGame.gameState.pendingNextLevel){
                if (CURRENT_LEVEL.hdcCache) { printf("DELETED DC"); DeleteDC(CURRENT_LEVEL.hdcCache);}
                if (CURRENT_LEVEL.hStaticCache) DeleteObject(CURRENT_LEVEL.hStaticCache);
                gGame.currentLevel++;
                ResetBetweenLevels(hwnd);
                InitLevel(hwnd);
            }else if(gGame.gameState.pendingRestart){
                gGame.gameState.currentMode = GAME_MODE_PLAYING;
            }

            gGame.transitionState = TRANSITION_OPENING;
        }
    }
     else if (gGame.transitionState == TRANSITION_OPENING)
    {
        gGame.animatedWalls.wallTopY -= gGame.animatedWalls.wallSpeed;
        gGame.animatedWalls.wallBottomY += gGame.animatedWalls.wallSpeed;

        if (gGame.animatedWalls.wallTopY <= -gGame.animatedWall.height)
        {


            gGame.transitionState = TRANSITION_NONE;
            gGame.gameState.pendingHome = false;
            gGame.gameState.pendingRestart = false;
            gGame.gameState.pendingNextLevel = false;
        }
    }
}

void UpdateLayout(int oldW, int oldH, int newW, int newH)
{
    if (oldW <= 0 || oldH <= 0)
        return;

    int oldBgX = gGame.leftWall.width;
    int oldBgW = oldW - gGame.leftWall.width - gGame.rightWall.width;
    int oldBgH = oldH - gGame.floorWall.height;

    int newBgX = gGame.leftWall.width;
    int newBgW = newW - gGame.leftWall.width - gGame.rightWall.width;
    int newBgH = newH - gGame.floorWall.height;

    float scaleX = (float)newBgW / (float)oldBgW;
    float scaleY = (float)newBgH / (float)oldBgH;

    Level& lvl = gGame.levels[gGame.currentLevel];

    for (int i = 0; i < MAX_BALLOONS; i++)
    {
        if(!lvl.balloons[i].active) continue;

        Balloon& b = lvl.balloons[i];

        int localX = b.x - oldBgX;
        int localY = b.y;

        localX = (int)(localX * scaleX);
        localY = (int)(localY * scaleY);

        b.x = newBgX + localX;
        b.y = localY;
    }

    // UPDATE HERO
    {
        int localX = gGame.hero.x - oldBgX;
        int localY = gGame.hero.y;

        localX = (int)(localX * scaleX);
        localY = (int)(localY * scaleY);

        gGame.hero.x = newBgX + localX;
        gGame.hero.y = localY;

        // opcionalno: update grounded Y
        gGame.hero.y = newH - gGame.floorWall.height - gGame.hero.height;
    }

    // UPDATE HARPOON

    if (gGame.harpoon.isActive)
    {
        int localX = gGame.harpoon.x - oldBgX;
        int localY = gGame.harpoon.y;

        localX = (int)(localX * scaleX);
        localY = (int)(localY * scaleY);

        gGame.harpoon.x = newBgX + localX;
        gGame.harpoon.y = localY;
    }

}


