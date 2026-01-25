#include "resources.h"
#include <windows.h>

void CheckInputs(HWND hwnd){
    RECT rect;
    GetClientRect(hwnd, &rect);
    int windowWidth = rect.right;

    bool isMoving = false;

    // === MOVEMENT ===
    if(GetAsyncKeyState(VK_LEFT)){
        hero.x -= hero.dx;
        hero.currentRow = 1;
        isMoving = true;
    }
    else if(GetAsyncKeyState(VK_RIGHT)){
        hero.x += hero.dx;
        hero.currentRow = 0;
        isMoving = true;
    }

    // === BOUNDARY CHECK ===
    if (hero.x < leftWall.width) {
        hero.x = leftWall.width;
    }

    int desnaGranica = windowWidth - rightWall.width - hero.width;
    if (hero.x > desnaGranica) {
        hero.x = desnaGranica;
    }

    // === ANIMATION ===
    if(isMoving) {
        hero.animCounter++;
        if(hero.animCounter > 5) {
            hero.currentFrame++;
            if(hero.currentFrame > 3) {
                hero.currentFrame = 0;
            }
            hero.animCounter = 0;
        }
    } else {
        hero.currentRow = 2;
        hero.currentFrame = 0;
    }

    // === HARPOON SHOOTING ===
    bool isSpacePressed = (GetAsyncKeyState(VK_SPACE) & 0x8000) != 0;

    if (isSpacePressed && !inputState.wasSpacePressed && !harpoon.isActive) {
        harpoon.isActive = true;
        harpoon.length = 0;
        harpoon.x = hero.x + (hero.width / 2) - (harpoon.width / 2);
        harpoon.y = rect.bottom - floorWall.height;
    }

    inputState.wasSpacePressed = isSpacePressed;
}
