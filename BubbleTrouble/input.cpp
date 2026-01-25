#include "resources.h"
#include <windows.h>

void CheckInputs(HWND hwnd) {
    bool isMoving = false;

    if (GetAsyncKeyState(VK_LEFT)) {
        hero.x -= hero.dx;
        hero.currentRow = 1;
        isMoving = true;
    } else if (GetAsyncKeyState(VK_RIGHT)) {
        hero.x += hero.dx;
        hero.currentRow = 0;
        isMoving = true;
    }

    if (hero.x < leftWall.width)
        hero.x = leftWall.width;

    int desnaGranica = windowWidth - rightWall.width - hero.width;
    if (hero.x > desnaGranica)
        hero.x = desnaGranica;

    if (isMoving) {
        hero.animCounter++;
        if (hero.animCounter > 5) {
            hero.currentFrame++;
            if (hero.currentFrame > 3)
                hero.currentFrame = 0;
            hero.animCounter = 0;
        }
    } else {
        hero.currentRow = 2;
        hero.currentFrame = 0;
    }

    bool isSpacePressed = (GetAsyncKeyState(VK_SPACE) & 0x8000) != 0;
    if (isSpacePressed && !wasSpacePressed && !isHarpoonActive) {
        isHarpoonActive = true;
        harpoon.length = 0;

        RECT rect;
        GetClientRect(hwnd, &rect);
        harpoon.x = hero.x + (hero.width / 2) - (harpoon.width / 2);
        harpoon.y = rect.bottom - floorWall.height;
    }
    wasSpacePressed = isSpacePressed;
}

