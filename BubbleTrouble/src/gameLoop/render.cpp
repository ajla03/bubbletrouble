#include "resources.h"
#include "gameContext.h"
#include "resourceManager.h"
#include "game.h"
#include <windows.h>
#include <algorithm>
#include <cstdio>



void RefreshScreen(HWND hwnd){
    HDC hdc = GetDC(hwnd);
    RECT rect;
    GetClientRect(hwnd, &rect);

    // Double buffering setup
    HDC hdcBuffer = CreateCompatibleDC(hdc);
    HBITMAP hbmBuffer = CreateCompatibleBitmap(hdc, rect.right, rect.bottom);
    HBITMAP oldBufferBmp = (HBITMAP)SelectObject(hdcBuffer, hbmBuffer);


    if(gGame.gameState.currentMode == GAME_MODE_MENU){
        RenderMenu(hdcBuffer, rect);
    }else{
        RenderWalls(hdcBuffer, rect);
        RenderLevel(hdcBuffer, rect);
        RenderGameUI(hdcBuffer, rect);
        DrawTransitionWalls(hdcBuffer, rect);
    }

    // === COPY TO SCREEN ===
    BitBlt(hdc, 0, 0, rect.right, rect.bottom, hdcBuffer, 0, 0, SRCCOPY);

    // Cleanup
    SelectObject(hdcBuffer, oldBufferBmp);
    DeleteObject(hbmBuffer);
    DeleteDC(hdcBuffer);
    ReleaseDC(hwnd, hdc);
}






void RefreshSound(){
}

