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

    UpdateHeroCoolDown(0.016f);

    UpdateBalloons(hwnd);

    UpdateHearts();

    CheckCollisions();

    UpdateScoreAnimation();

    UpdateWallTransition(hwnd);
}

void CheckHover(Button& button, int mx, int my){
 if (mx >= button.x && mx <= button.x + button.width &&
            my >= button.y  && my <= button.y + button.height)
        {
            button.isHover = true;
        }
        else
        {
            button.isHover = false;
        }
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
            }else if(gGame.gameState.pendingNextLevel){
                gGame.currentLevel++;
                ResetBetweenLevels(hwnd);
                InitLevel(hwnd);
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
