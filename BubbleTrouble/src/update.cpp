#include "resources.h"
#include <windows.h>

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
    UpdateBalloons(hwnd);

    CheckCollisions();
}
