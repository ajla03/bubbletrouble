#include "resources.h"
#include "gameContext.h"
#include "resourceManager.h"
#include "game.h"
#include "loading.h"
#include <windows.h>
#include <algorithm>
#include <cstdio>


void RefreshScreen(HWND hwnd) {
    HDC hdc = GetDC(hwnd);
    RECT rect;
    GetClientRect(hwnd, &rect);
 if(gGame.gameState.currentMode == GAME_MODE_LOADING) {
        UpdateLoading();

        // DEBUG: Provjeri da li je buffer validan
        if (gRes.hdcBuffer == NULL) {
            // Privremeno crtaj direktno na ekran
            RenderLoading(hdc, rect);
        } else {
            RenderLoading(gRes.hdcBuffer, rect);
            BitBlt(hdc, 0, 0, rect.right, rect.bottom, gRes.hdcBuffer, 0, 0, SRCCOPY);
        }

        if(IsLoadingComplete()) {
            gGame.gameState.currentMode = GAME_MODE_LOGIN;
            InitLoginInput();
        }
        ReleaseDC(hwnd, hdc);
        return;
    }
    else if(gGame.gameState.currentMode == GAME_MODE_MENU) {
        InitializeMenu(hwnd);
        RenderMenu(gRes.hdcBuffer, rect);
        DrawTransitionWalls(gRes.hdcBuffer, rect);

    } else if(gGame.gameState.currentMode == GAME_MODE_SETTINGS){
       RenderSettings(gRes.hdcBuffer, rect);
       DrawTransitionWalls(gRes.hdcBuffer, rect);

    }
    else if(gGame.gameState.currentMode == GAME_MODE_HELP){
       RenderHelp(gRes.hdcBuffer, rect);
       DrawTransitionWalls(gRes.hdcBuffer, rect);
    }
   else if(gGame.gameState.currentMode == GAME_MODE_LOGIN){
        RenderLoginScreen(gRes.hdcBuffer, rect, hwnd);
        DrawTransitionWalls(gRes.hdcBuffer, rect);

   }else if(gGame.gameState.currentMode == GAME_MODE_DASHBOARD){
        RenderDashboard(gRes.hdcBuffer, rect);
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
