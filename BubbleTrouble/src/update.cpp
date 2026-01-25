#include "resources.h"
#include <windows.h>

void Update(HWND hwnd){
     if (!isGameOver && timeLeft > 0) {
        timeLeft -= 1.0;
    } else if (timeLeft <= 0) {
        timeLeft = 0;
        isGameOver = true;
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
