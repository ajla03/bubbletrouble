#include "game.h"
#include "resourceManager.h"
#include "gameContext.h"
#include <stdio.h>

extern void CheckHeroPillarCollision(Hero* h, StaticObject* pillar);
static bool s_pillarsActive = true;

void UpdateLevel8Pillars() {
    if (!s_pillarsActive) return;

    CheckHeroPillarCollision(&gGame.hero, &CURRENT_LEVEL.pillar1);
    CheckHeroPillarCollision(&gGame.hero, &CURRENT_LEVEL.pillar2);

    if(gGame.gameState.isMultiplayer) {
        CheckHeroPillarCollision(&gGame.hero2, &CURRENT_LEVEL.pillar1);
        CheckHeroPillarCollision(&gGame.hero2, &CURRENT_LEVEL.pillar2);
    }

    if (CURRENT_LEVEL.levelScore > 0) {
        s_pillarsActive = false;
        CURRENT_LEVEL.pillar1.width  = 0;
        CURRENT_LEVEL.pillar1.height = 0;
        CURRENT_LEVEL.pillar2.width  = 0;
        CURRENT_LEVEL.pillar2.height = 0;
    }
}

void RenderLevel8(HDC hdcBuffer, RECT rect){
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

void InitLevel8(HWND hwnd){
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

    CURRENT_LEVEL.pillar1.originalWidth  = bmp.bmWidth;
    CURRENT_LEVEL.pillar1.originalHeight = bmp.bmHeight;
    CURRENT_LEVEL.pillar1.width          = bmp.bmWidth;
    CURRENT_LEVEL.pillar1.height         = bmp.bmHeight;

    CURRENT_LEVEL.pillar2.originalWidth  = bmp.bmWidth;
    CURRENT_LEVEL.pillar2.originalHeight = bmp.bmHeight;
    CURRENT_LEVEL.pillar2.width          = bmp.bmWidth;
    CURRENT_LEVEL.pillar2.height         = bmp.bmHeight;

    RecalculateLevel8Layout(hwnd);

    s_pillarsActive = true;
    CURRENT_LEVEL.levelScore = 0;

    for(int i = 0; i < MAX_BALLOONS; i++)
        CURRENT_LEVEL.balloons[i].active = false;


    int bgW = rect.right - gGame.leftWall.width - gGame.rightWall.width;

    int pillar1CenterX = gGame.leftWall.width + bgW / 4;
    int pillar2CenterX = gGame.leftWall.width + 3 * bgW / 4;

    float section1CenterX = gGame.leftWall.width + (float)(bgW / 4) / 2.0f;
    InitBalloon(0, section1CenterX, 120.0f, 80.0f, -3.0f, RGB(220, 30, 30));

    float section2CenterX = (float)(pillar1CenterX + pillar2CenterX) / 2.0f;
    InitBalloon(1, section2CenterX, 130.0f, 65.0f,  2.8f, RGB(255, 210, 0));

    float section3CenterX = pillar2CenterX + (float)(bgW / 4) / 2.0f;
    InitBalloon(2, section3CenterX, 120.0f, 80.0f, -3.0f, RGB(255, 130, 0));


    int centerX = rect.right / 2;
    gGame.hero.x = centerX - gGame.hero.width - 20;
    gGame.hero.y = rect.bottom - gGame.floorWall.height - gGame.hero.height;
    gGame.hero.floorY = 0;
    gGame.hero.isOnLadder = false;

    if(gGame.gameState.isMultiplayer) {
        gGame.hero2.x = centerX + 20;
        gGame.hero2.y = rect.bottom - gGame.floorWall.height - gGame.hero2.height;
        gGame.hero2.floorY = 0;
        gGame.hero2.isOnLadder = false;
    }

    InitPowerupSystemForLevel();

}

void RecalculateLevel8Layout(HWND hwnd)
{
    RECT rect;
    GetClientRect(hwnd, &rect);

    int bgW = rect.right - gGame.leftWall.width - gGame.rightWall.width;
    int bgH = rect.bottom - gGame.floorWall.height;


    CURRENT_LEVEL.pillar1.x      = gGame.leftWall.width + bgW / 4 - CURRENT_LEVEL.pillar1.width / 2;
    CURRENT_LEVEL.pillar1.y      = 0;
    CURRENT_LEVEL.pillar1.height = bgH;

    CURRENT_LEVEL.pillar2.x      = gGame.leftWall.width + 3 * bgW / 4 - CURRENT_LEVEL.pillar2.width / 2;
    CURRENT_LEVEL.pillar2.y      = 0;
    CURRENT_LEVEL.pillar2.height = bgH;
}
