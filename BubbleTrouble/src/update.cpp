#include "resources.h"
#include "globals.h"
#include "game.h"
#include <windows.h>

void UpdateHeroCoolDown(float dt){
    if(hero.heroHitCooldown > 0)
        hero.heroHitCooldown -=dt;
}

void UpdateHearts(){
 for (int i = 0; i < gameState.lives; i++) {
    hearts[i].animCounter++;

    if (hearts[i].animCounter >= HEART_ANIM_SPEED) {
        hearts[i].animCounter = 0;
        hearts[i].currentFrame++;

        if (hearts[i].currentFrame >= HEART_FRAMES) {
            hearts[i].currentFrame = 0;
        }
    }
 }
}

void Update(HWND hwnd){
     if (!gameState.isGameOver && gameState.timeLeft > 0 && !gameState.isLevelCleared) {
        gameState.timeLeft -= 1.0;
    } else if (gameState.timeLeft <= 0) {
        gameState.timeLeft = 0;
        gameState.isGameOver = true;
    }
    if (harpoon.isActive){
        RECT rect;
        GetClientRect(hwnd, &rect);
        harpoon.length += harpoon.dy;
        harpoon.y = rect.bottom - floorWall.height;

        if (harpoon.length >= rect.bottom){
            harpoon.isActive = false;
        }
    }

    // update baklji
    torchInfo.animCounter++;
    if(torchInfo.animCounter > 5){
    torchInfo.currentFrame++;
    if (torchInfo.currentFrame >= 4) {
        torchInfo.currentFrame = 0;
        torchInfo.currentRow = (torchInfo.currentRow + 1) % 2;
     }
     torchInfo.animCounter = 0;
    }

    UpdateHeroCoolDown(0.016f);

    UpdateBalloons(hwnd);

    UpdateHearts();

    CheckCollisions();

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
    if(transitionState == TRANSITION_CLOSING){
        animatedWalls.wallTopY +=animatedWalls.wallSpeed;
        animatedWalls.wallBottomY -=animatedWalls.wallSpeed;

        if(animatedWalls.wallTopY + animatedWall.height >= animatedWalls.wallBottomY){

            if(gameState.pendingRestart){
                ResetGame(hwnd);
            }

            animatedWalls.transitionWaitStart = GetTickCount();
            transitionState = TRANSITION_WAIT;
        }
     }else if (transitionState == TRANSITION_WAIT)
    {
        if (GetTickCount() - animatedWalls.transitionWaitStart >= 400)
        {
            if(gameState.pendingHome){
                gameState.currentMode = GAME_MODE_MENU;
                gameState.isGameOver = false;
                gameState.isLevelCleared = false;
            }

            transitionState = TRANSITION_OPENING;
        }
    }
     else if (transitionState == TRANSITION_OPENING)
    {
        animatedWalls.wallTopY -= animatedWalls.wallSpeed;
        animatedWalls.wallBottomY += animatedWalls.wallSpeed;

        if (animatedWalls.wallTopY <= -animatedWall.height)
        {

            transitionState = TRANSITION_NONE;
            gameState.pendingHome = false;
            gameState.pendingRestart = false;
        }
    }
}
