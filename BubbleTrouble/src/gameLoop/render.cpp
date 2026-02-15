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

    const auto& mode = gGame.gameState.currentMode;

    if (mode == GAME_MODE_LOADING) {
        UpdateLoading();

        if (gRes.hdcBuffer == NULL) {
            RenderLoading(hdc, rect);
        } else {
            RenderLoading(gRes.hdcBuffer, rect);
            BitBlt(hdc, 0, 0, rect.right, rect.bottom, gRes.hdcBuffer, 0, 0, SRCCOPY);
        }

        if (IsLoadingComplete()) {
            gGame.gameState.currentMode = GAME_MODE_LOGIN;
            InitLoginInput();
        }
        ReleaseDC(hwnd, hdc);
        return;
    }

    switch (mode) {
        case GAME_MODE_MENU:
            InitializeMenu(hwnd);
            RenderMenu(gRes.hdcBuffer, rect);
            break;

        case GAME_MODE_SETTINGS:
            RenderSettings(gRes.hdcBuffer, rect);
            break;

        case GAME_MODE_HELP:
            RenderHelp(gRes.hdcBuffer, rect);
            break;

        case GAME_MODE_LOGIN:
            RenderLoginScreen(gRes.hdcBuffer, rect, hwnd);
            break;

        case GAME_MODE_DASHBOARD:
            RenderDashboard(gRes.hdcBuffer, rect);
            break;

        default: // GAME_MODE_PLAYING, PAUSE, GAME_OVER
            RenderStaticUI(gRes.hdcBuffer, rect);
            RenderLevel(gRes.hdcBuffer, rect);
            RenderDynamicGameUI(gRes.hdcBuffer, rect);
            break;
    }

    DrawTransitionWalls(gRes.hdcBuffer, rect);

    BitBlt(hdc, 0, 0, rect.right, rect.bottom, gRes.hdcBuffer, 0, 0, SRCCOPY);

    ReleaseDC(hwnd, hdc);
}



