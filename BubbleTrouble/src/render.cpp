#include "resources.h"
#include <windows.h>
#include <algorithm>

void RefreshScreen(HWND hwnd){
    HDC hdc = GetDC(hwnd);
    RECT rect;
    GetClientRect(hwnd, &rect);

    // Double buffering setup
    HDC hdcBuffer = CreateCompatibleDC(hdc);
    HDC hdcMem = CreateCompatibleDC(hdc);
    HBITMAP hbmBuffer = CreateCompatibleBitmap(hdc, rect.right, rect.bottom);
    HBITMAP oldBufferBmp = (HBITMAP)SelectObject(hdcBuffer, hbmBuffer);

    // === RENDER WALLS ===
    SelectObject(hdcMem, wall);

    // Left wall
    for (int y = 0; y < rect.bottom; y += leftWall.height) {
        BitBlt(hdcBuffer, 0, y, leftWall.width, leftWall.height, hdcMem, 0, 0, SRCCOPY);
    }

    // Right wall
    for (int y = 0; y < rect.bottom; y += rightWall.height) {
        BitBlt(hdcBuffer, rect.right - rightWall.width, y, rightWall.width, rightWall.height, hdcMem, 0, 0, SRCCOPY);
    }

    // Floor
    StretchBlt(hdcBuffer, 0, rect.bottom - floorWall.height, rect.right, floorWall.height,
               hdcMem, 0, 0, floorWall.width, floorWall.height, SRCCOPY);

    // === RENDER BACKGROUND ===
    SelectObject(hdcMem, background);

    int bgX = leftWall.width;
    int bgY = 0;
    int bgW = rect.right - leftWall.width - rightWall.width;
    int bgH = rect.bottom - floorWall.height;

    StretchBlt(hdcBuffer, bgX, bgY, bgW, bgH, hdcMem, 0, 0, backgroundInfo.width, backgroundInfo.height, SRCCOPY);

     for (int i = 0; i < MAX_BALLOONS; i++) {
        if (balloons[i].active) {
            DrawBalloonGDI(hdcBuffer, &balloons[i]);
        }
    }

    // === RENDER HARPOON ===
    if (harpoon.isActive) {
        int visiblePart = std::min(harpoon.length, harpoon.height);
        int destY = rect.bottom - floorWall.height - visiblePart;

        SelectObject(hdcMem, arrowMask);
        StretchBlt(hdcBuffer, harpoon.x, destY, harpoon.width, visiblePart,
                   hdcMem, 0, 0, harpoon.width, visiblePart, SRCPAINT);

        SelectObject(hdcMem, arrow);
        StretchBlt(hdcBuffer, harpoon.x, destY, harpoon.width, visiblePart,
                   hdcMem, 0, 0, harpoon.width, visiblePart, SRCAND);
    }

    // === RENDER HERO ===
    int heroY = rect.bottom - floorWall.height - hero.height;
    int srcX = hero.currentFrame * hero.width;
    int srcY = hero.currentRow * hero.height;

    SelectObject(hdcMem, characterMask);
    BitBlt(hdcBuffer, hero.x, heroY, hero.width, hero.height, hdcMem, srcX, srcY, SRCAND);

    SelectObject(hdcMem, character);
    BitBlt(hdcBuffer, hero.x, heroY, hero.width, hero.height, hdcMem, srcX, srcY, SRCPAINT);

    // === COPY TO SCREEN ===
    BitBlt(hdc, 0, 0, rect.right, rect.bottom, hdcBuffer, 0, 0, SRCCOPY);

    // Cleanup
    SelectObject(hdcBuffer, oldBufferBmp);
    DeleteObject(hbmBuffer);
    DeleteDC(hdcMem);
    DeleteDC(hdcBuffer);
    ReleaseDC(hwnd, hdc);
}

void RefreshSound(){
}

