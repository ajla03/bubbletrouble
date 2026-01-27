#include "resources.h"
#include <windows.h>

void UpdateHeroCoolDown(float dt){
    if(hero.heroHitCooldown > 0)
        hero.heroHitCooldown -=dt;
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

    CheckCollisions();
}
