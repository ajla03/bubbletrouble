#include "resources.h"
#include "gameContext.h"
#include "constants.h"
#include <windows.h>


void CheckInputs(HWND hwnd){
    if(gGame.gameState.isGameOver || gGame.gameState.isLevelCleared)
        return;

    RECT rect;
    GetClientRect(hwnd, &rect);
    int windowWidth = rect.right;

    bool isMoving = false;

    // === MOVEMENT ===
    if(GetAsyncKeyState(VK_LEFT)){
        gGame.hero.x -= gGame.hero.dx;
        gGame.hero.currentRow = 1;
        isMoving = true;
    }
    else if(GetAsyncKeyState(VK_RIGHT)){
        gGame.hero.x += gGame.hero.dx;
        gGame.hero.currentRow = 0;
        isMoving = true;
    }

    // === BOUNDARY CHECK ===
    if (gGame.hero.x < gGame.leftWall.width) {
        gGame.hero.x = gGame.leftWall.width;
    }

    int desnaGranica = windowWidth - gGame.rightWall.width - gGame.hero.width;
    if (gGame.hero.x > desnaGranica) {
        gGame.hero.x = desnaGranica;
    }

    // === ANIMATION ===
    if(isMoving) {
        gGame.hero.animCounter++;
        if(gGame.hero.animCounter > 5) {
            gGame.hero.currentFrame++;
            if(gGame.hero.currentFrame > 3) {
                gGame.hero.currentFrame = 0;
            }
            gGame.hero.animCounter = 0;
        }
    } else {
        gGame.hero.currentRow = 2;
        gGame.hero.currentFrame = 0;
    }

    // === HARPOON SHOOTING ===
    bool isSpacePressed = (GetAsyncKeyState(VK_SPACE) & 0x8000) != 0;

    if (isSpacePressed && !gGame.inputState.wasSpacePressed && !gGame.harpoon.isActive) {
        gGame.harpoon.isActive = true;
        gGame.harpoon.length = 0;
        gGame.harpoon.x = gGame.hero.x + (gGame.hero.width / 2) - (gGame.harpoon.width / 2);
        gGame.harpoon.y = rect.bottom - gGame.floorWall.height;
        PlaySound(MAKEINTRESOURCE(IDR_HARPOON_SOUND), GetModuleHandle(NULL),
              SND_RESOURCE | SND_ASYNC);
    }

    gGame.inputState.wasSpacePressed = isSpacePressed;
}
