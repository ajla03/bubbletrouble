#include "game.h"
#include "resourceManager.h"
#include "gameContext.h"
#include <stdio.h>

void RenderLevel4(HDC hdcBuffer, RECT rect){
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

    if(CURRENT_LEVEL.pillar1.width > 0){
        SelectObject(gRes.hdcMem, gRes.longWall);
        SetStretchBltMode(hdcBuffer, HALFTONE);
        SetBrushOrgEx(hdcBuffer, 0, 0, NULL);
        StretchBlt(
            hdcBuffer,
            CURRENT_LEVEL.pillar1.x,
            CURRENT_LEVEL.pillar1.y,
            CURRENT_LEVEL.pillar1.width,
            CURRENT_LEVEL.pillar1.height,
            gRes.hdcMem,
            0, 0,
            CURRENT_LEVEL.pillar1.originalWidth,
            CURRENT_LEVEL.pillar1.originalHeight,
            SRCCOPY
        );
    }

    if(CURRENT_LEVEL.pillar2.width > 0){
        SelectObject(gRes.hdcMem, gRes.longWall);
        SetStretchBltMode(hdcBuffer, HALFTONE);
        SetBrushOrgEx(hdcBuffer, 0, 0, NULL);
        StretchBlt(
            hdcBuffer,
            CURRENT_LEVEL.pillar2.x,
            CURRENT_LEVEL.pillar2.y,
            CURRENT_LEVEL.pillar2.width,
            CURRENT_LEVEL.pillar2.height,
            gRes.hdcMem,
            0, 0,
            CURRENT_LEVEL.pillar2.originalWidth,
            CURRENT_LEVEL.pillar2.originalHeight,
            SRCCOPY
        );
    }

    for (int i = 0; i < MAX_BALLOONS; i++) {
        if (CURRENT_LEVEL.balloons[i].active) {
            DrawBalloonGDI(hdcBuffer, &CURRENT_LEVEL.balloons[i]);
        }
    }

    DrawPowerups(hdcBuffer, rect);

}

void InitLevel4(HWND hwnd){
    RECT rect;
    GetClientRect(hwnd, &rect);

    if(gRes.hBgLevel2){
        BITMAP bm;
        GetObject(gRes.hBgLevel2, sizeof(BITMAP), &bm);
        CURRENT_LEVEL.backgroundInfo.width = bm.bmWidth;
        CURRENT_LEVEL.backgroundInfo.height = bm.bmHeight;
    }

    BITMAP bmp;
    GetObject(gRes.longWall, sizeof(BITMAP), &bmp);

    CURRENT_LEVEL.pillar1.originalWidth = bmp.bmWidth;
    CURRENT_LEVEL.pillar1.originalHeight = bmp.bmHeight;
    CURRENT_LEVEL.pillar1.width = bmp.bmWidth;
    CURRENT_LEVEL.pillar1.height = bmp.bmHeight;

    CURRENT_LEVEL.pillar2.originalWidth = bmp.bmWidth;
    CURRENT_LEVEL.pillar2.originalHeight = bmp.bmHeight;
    CURRENT_LEVEL.pillar2.width = bmp.bmWidth;
    CURRENT_LEVEL.pillar2.height = bmp.bmHeight;

    RecalculateLevel4Layout(hwnd);

    for(int i = 0; i < MAX_BALLOONS; i++)
        CURRENT_LEVEL.balloons[i].active = false;

    int bgW = rect.right - gGame.leftWall.width - gGame.rightWall.width;
    int sectionWidth = bgW / 3;

    float section1CenterX = gGame.leftWall.width + sectionWidth / 2;
    InitBalloon(0, section1CenterX, 100, 20, 2.0f, RGB(255, 200, 0));

    float section2CenterX = gGame.leftWall.width + sectionWidth + sectionWidth / 2;
    InitBalloon(1, section2CenterX, 150, 40, -2.5f, RGB(255, 100, 0));

    float section3CenterX = gGame.leftWall.width + 2 * sectionWidth + sectionWidth / 2;
    InitBalloon(2, section3CenterX, 120, 80, 3.0f, RGB(255, 0, 0));
    InitPowerupSystemForLevel();
    if (gGame.gameState.isMultiplayer) {

        gGame.hero.x = gGame.leftWall.width + 50;
        gGame.hero2.x = gGame.leftWall.width + 130;


        RECT rect;
        GetClientRect(hwnd, &rect);
        gGame.hero2.y = rect.bottom - gGame.floorWall.height - gGame.hero2.height;
    }

}

void RecalculateLevel4Layout(HWND hwnd)
{
    RECT rect;
    GetClientRect(hwnd, &rect);

    int bgW = rect.right - gGame.leftWall.width - gGame.rightWall.width;
    int bgH = rect.bottom - gGame.floorWall.height;
    int sectionWidth = bgW / 3;

    CURRENT_LEVEL.pillar1.x = gGame.leftWall.width + sectionWidth - CURRENT_LEVEL.pillar1.width / 2;
    CURRENT_LEVEL.pillar1.y = 0;
    CURRENT_LEVEL.pillar1.height = bgH;

    CURRENT_LEVEL.pillar2.x = gGame.leftWall.width + 2 * sectionWidth - CURRENT_LEVEL.pillar2.width / 2;
    CURRENT_LEVEL.pillar2.y = 0;
    CURRENT_LEVEL.pillar2.height = bgH;
}
