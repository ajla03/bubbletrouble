#include "game.h"
#include "resourceManager.h"
#include "gameContext.h"
#include <stdio.h>

void RenderLevel6(HDC hdcBuffer, RECT rect){
    HDC hdcMem = CreateCompatibleDC(hdcBuffer);

    SelectObject(hdcMem, gRes.hBgLevel2);

    int bgX = gGame.leftWall.width;
    int bgY = 0;
    int bgW = rect.right - gGame.leftWall.width - gGame.rightWall.width;
    int bgH = rect.bottom - gGame.floorWall.height;

    SetStretchBltMode(hdcBuffer, HALFTONE);
    SetBrushOrgEx(hdcBuffer, 0, 0, NULL);
    StretchBlt(
        hdcBuffer, bgX, bgY, bgW, bgH,
        hdcMem, 0, 0,
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

    DeleteDC(hdcMem);
}

void InitLevel6(HWND hwnd){
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


    int yPosition = 120;
    int spacingX = 50;
    int radius = 20;

    int leftStartX = gGame.leftWall.width + 50;

    int rightStartX = rect.right - gGame.rightWall.width - 50;

    COLORREF colors[2] = {
        RGB(0, 255, 0),
        RGB(255, 255, 0)
    };

    for(int i = 0; i < 4; i++) {

        InitBalloon(
            i,
            leftStartX + (i * spacingX),
            yPosition,
            radius,
            2.0f,
            colors[i % 2]
        );


        InitBalloon(
            i + 4,
            rightStartX - (i * spacingX),
            yPosition,
            radius,
            -2.0f,
            colors[(i + 1) % 2]
        );
    }

    InitPowerupSystemForLevel();

}
