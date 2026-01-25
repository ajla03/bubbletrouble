#include "resources.h"
#include <windows.h>

void Update(HWND hwnd){
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
