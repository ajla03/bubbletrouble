#include "resources.h"
#include "gameContext.h"
#include "resourceManager.h"
#include "game.h"
#include <windows.h>
#include <algorithm>
#include <cstdio>


void RefreshScreen(HWND hwnd) {
    HDC hdc = GetDC(hwnd);
    RECT rect;
    GetClientRect(hwnd, &rect);

    if(gGame.gameState.currentMode == GAME_MODE_MENU) {
        InitializeMenu(hwnd);
        RenderMenu(gRes.hdcBuffer, rect);
        DrawTransitionWalls(gRes.hdcBuffer, rect);
    } else if(gGame.gameState.currentMode == GAME_MODE_SETTINGS){
       RenderSettings(gRes.hdcBuffer, rect);
       DrawTransitionWalls(gRes.hdcBuffer, rect);
    }else {
        RenderStaticUI(gRes.hdcBuffer, rect);
        RenderLevel(gRes.hdcBuffer, rect);
        RenderDynamicGameUI(gRes.hdcBuffer, rect);
        DrawTransitionWalls(gRes.hdcBuffer, rect);
    }

    BitBlt(hdc, 0, 0, rect.right, rect.bottom, gRes.hdcBuffer, 0, 0, SRCCOPY);

    ReleaseDC(hwnd, hdc);
}



void RefreshSound(){
}

