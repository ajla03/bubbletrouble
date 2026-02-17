#include "game.h"
#include "resourceManager.h"
#include "gameContext.h"
#include <stdio.h>

void RenderLevel8(HDC hdcBuffer, RECT rect){
    // === BACKGROUND ===
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

    // === FIRST PILLAR (LEFT) ===
    // Both pillars disappear simultaneously (controlled via UpdateLevelLogic)
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

    // === SECOND PILLAR (RIGHT) ===
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

    // === BALLOONS ===
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

    // === LOAD BACKGROUND INFO ===
    if(gRes.hBgLevel2){
        BITMAP bm;
        GetObject(gRes.hBgLevel2, sizeof(BITMAP), &bm);
        CURRENT_LEVEL.backgroundInfo.width = bm.bmWidth;
        CURRENT_LEVEL.backgroundInfo.height = bm.bmHeight;
    }

    // === SETUP PILLARS ===
    BITMAP bmp;
    GetObject(gRes.longWall, sizeof(BITMAP), &bmp);

    // Pillar 1 (Left divider)
    CURRENT_LEVEL.pillar1.originalWidth  = bmp.bmWidth;
    CURRENT_LEVEL.pillar1.originalHeight = bmp.bmHeight;
    CURRENT_LEVEL.pillar1.width          = bmp.bmWidth;
    CURRENT_LEVEL.pillar1.height         = bmp.bmHeight;

    // Pillar 2 (Right divider)
    CURRENT_LEVEL.pillar2.originalWidth  = bmp.bmWidth;
    CURRENT_LEVEL.pillar2.originalHeight = bmp.bmHeight;
    CURRENT_LEVEL.pillar2.width          = bmp.bmWidth;
    CURRENT_LEVEL.pillar2.height         = bmp.bmHeight;

    RecalculateLevel8Layout(hwnd);

    // === CLEAR ALL BALLOONS ===
    for(int i = 0; i < MAX_BALLOONS; i++)
        CURRENT_LEVEL.balloons[i].active = false;

    // === CREATE 3 BALLOONS - ONE PER SECTION ===
    // Layout: [  Section1  |P1|  Section2  |P2|  Section3  ]
    //
    // From the reference image:
    //   Section 1 (Left)   - Large RED balloon,    radius 80
    //   Section 2 (Middle) - Medium YELLOW balloon, radius 55
    //   Section 3 (Right)  - Medium ORANGE balloon, radius 65
    //
    // MECHANIC: Both pillars disappear SIMULTANEOUSLY when
    //           ALL balloons in Section1 AND Section3 are destroyed.

    int bgW = rect.right - gGame.leftWall.width - gGame.rightWall.width;
    int sectionWidth = bgW / 3;

    // Section 1 (Left) - Large red balloon
    float section1CenterX = gGame.leftWall.width + sectionWidth / 2.0f;
    InitBalloon(0, section1CenterX, 120.0f, 80.0f, -3.0f, RGB(220, 30, 30));   // Red

    // Section 2 (Middle) - Medium yellow balloon
    float section2CenterX = gGame.leftWall.width + sectionWidth + sectionWidth / 2.0f;
    InitBalloon(1, section2CenterX, 150.0f, 55.0f,  2.5f, RGB(255, 210, 0));   // Yellow

    // Section 3 (Right) - Medium-large orange balloon
    float section3CenterX = gGame.leftWall.width + 2 * sectionWidth + sectionWidth / 2.0f;
    InitBalloon(2, section3CenterX, 130.0f, 65.0f, -2.8f, RGB(255, 130, 0));   // Orange

    InitPowerupSystemForLevel();

    // === MULTIPLAYER POSITIONING ===
    if (gGame.gameState.isMultiplayer) {
        gGame.hero.x  = gGame.leftWall.width + 50;
        gGame.hero2.x = gGame.leftWall.width + 130;

        RECT r;
        GetClientRect(hwnd, &r);
        gGame.hero2.y = r.bottom - gGame.floorWall.height - gGame.hero2.height;
    }
}

void RecalculateLevel8Layout(HWND hwnd)
{
    RECT rect;
    GetClientRect(hwnd, &rect);

    int bgW = rect.right - gGame.leftWall.width - gGame.rightWall.width;
    int bgH = rect.bottom - gGame.floorWall.height;
    int sectionWidth = bgW / 3;

    // === PILLAR 1 (divides section 1 and 2) ===
    CURRENT_LEVEL.pillar1.x      = gGame.leftWall.width + sectionWidth - CURRENT_LEVEL.pillar1.width / 2;
    CURRENT_LEVEL.pillar1.y      = 0;
    CURRENT_LEVEL.pillar1.height = bgH;

    // === PILLAR 2 (divides section 2 and 3) ===
    CURRENT_LEVEL.pillar2.x      = gGame.leftWall.width + 2 * sectionWidth - CURRENT_LEVEL.pillar2.width / 2;
    CURRENT_LEVEL.pillar2.y      = 0;
    CURRENT_LEVEL.pillar2.height = bgH;
}
