#include "game.h"
#include "resourceManager.h"
#include "gameContext.h"
#include <stdio.h>

void RenderLevel5(HDC hdcBuffer, RECT rect){
    SelectObject(gRes.hdcMem, gRes.hBgLevel2);

    int bgX = gGame.leftWall.width;
    int bgY = 0;
    int bgW = rect.right - gGame.leftWall.width - gGame.rightWall.width;
    int bgH = rect.bottom - gGame.floorWall.height;

    SetStretchBltMode(hdcBuffer, HALFTONE);
    SetBrushOrgEx(hdcBuffer, 0, 0, NULL);
    StretchBlt(
        hdcBuffer, bgX, bgY, bgW, bgH,
        gRes.hdcMem, 0, 0,
        CURRENT_LEVEL.backgroundInfo.width,
        CURRENT_LEVEL.backgroundInfo.height,
        SRCCOPY
    );

    for (int i = 0; i < MAX_BALLOONS; i++) {
        if (CURRENT_LEVEL.balloons[i].active) {
            DrawBalloonGDI(hdcBuffer, &CURRENT_LEVEL.balloons[i]);
        }
    }
    DrawPowerups(hdcBuffer, rect);
}

void InitLevel5(HWND hwnd){
    RECT rect;
    GetClientRect(hwnd, &rect);

    if(gRes.hBgLevel2){
        BITMAP bm;
        GetObject(gRes.hBgLevel2, sizeof(BITMAP), &bm);
        CURRENT_LEVEL.backgroundInfo.width = bm.bmWidth;
        CURRENT_LEVEL.backgroundInfo.height = bm.bmHeight;
    }

    for(int i = 0; i < MAX_BALLOONS; i++)
        CURRENT_LEVEL.balloons[i].active = false;

    int bgW = rect.right - gGame.leftWall.width - gGame.rightWall.width;
    int bgH = rect.bottom - gGame.floorWall.height;

    float centerX = gGame.leftWall.width + bgW / 2;
    float centerY = bgH / 2;

    float yellowRadius = 50.0f;
    float redRadius = 80.0f;

    float yellowLeftX = gGame.leftWall.width + bgW / 4;
    float yellowRightX = gGame.leftWall.width + 3 * bgW / 4;

    InitBalloon(0, yellowLeftX, 150, yellowRadius, 2.0f, RGB(255, 255, 0));

    InitBalloon(1, centerX, centerY, redRadius, 0.0f, RGB(255, 0, 0));

    InitBalloon(2, yellowRightX, 150, yellowRadius, -2.0f, RGB(255, 255, 0));
    InitPowerupSystemForLevel();

}
